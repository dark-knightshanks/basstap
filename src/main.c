#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>
#include <string.h>
#include <stdio.h>

int source_found = 0;
pa_mainloop *m_pulseaudio_mainloop;

void print_banner(void) { 
	printf("\n"); 
	printf("██████╗ █████╗ ███████╗███████╗████████╗ █████╗  █████╗  ██████╗ \n"); 
	printf("██╔══██╗██╔══██╗██╔════╝██╔════╝╚██╔══╝ ██╔══██╗██╔══██╗ ██╔══██╗\n"); 
	printf("██████╔╝███████║███████╗███████╗ ██║    ███████║███████║ ██████╔╝\n"); 
	printf("██╔══██╗██╔══██║╚════██║╚════██║ ██║    ██╔══██║██╔══██║ ██╔═══╝ \n"); 
	printf("██████╔╝██║ ██║███████║███████║  ██║    ██║  ██║██║  ██║ ██║ \n"); 
	printf("╚═════╝ ╚═╝ ╚═╝╚══════╝╚══════╝  ╚═╝    ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═╝ \n"); 
	printf("\n"); printf(" PulseAudio Terminal Visualizer\n"); 
	printf("--------------------------------------------------\n\n"); }

/*callback function that pulseaudio uses to give the source of the audio from user data*/
void cb(__attribute__((unused))pa_context *pulseaudio_context,
		const pa_server_info *i,
		void *userdata){
	
	char *source = (char *)userdata;
	strcpy(source, i->default_sink_name);
	strcat(source, ".monitor");
	printf("Monitor source: %s\n", source);
	source_found = 1;
    pa_mainloop_quit(
        m_pulseaudio_mainloop,
        0
    );
}

void context_state_callback(
    pa_context *context,
    void *userdata
)
{
    pa_context_state_t state = pa_context_get_state(context);

    switch (state) {

        case PA_CONTEXT_READY:
            printf("PulseAudio context is ready!\n");

            pa_context_get_server_info(
                context,
                cb,
                userdata
            );

            break;

        case PA_CONTEXT_FAILED:
            fprintf(stderr, "PulseAudio connection failed\n");
            pa_mainloop_quit(m_pulseaudio_mainloop, 1);
            break;

        case PA_CONTEXT_TERMINATED:
            pa_mainloop_quit(m_pulseaudio_mainloop, 1);
            break;

        default:
            break;
    }
}

int main (){
	print_banner();
	char source[1024] = {0};
	size_t buffer_size = 4096;
	unsigned char buf[buffer_size];

    m_pulseaudio_mainloop = pa_mainloop_new();

    if (m_pulseaudio_mainloop == NULL) {
        fprintf(stderr, "Failed to create PulseAudio mainloop\n");
        return 1;
    }

    pa_mainloop_api *api =
        pa_mainloop_get_api(m_pulseaudio_mainloop);



    pa_context *context =
        pa_context_new(api, "basstap");

    if (context == NULL) {
        fprintf(stderr, "Failed to create PulseAudio context\n");
        pa_mainloop_free(m_pulseaudio_mainloop);
        return 1;
    }



    

    if (pa_context_connect(context, NULL, 0, NULL) < 0) {
        fprintf(stderr, "Failed to connect to PulseAudio\n");

        pa_context_unref(context);
        pa_mainloop_free(m_pulseaudio_mainloop);

        return 1;
    }

    pa_context_set_state_callback(
    context,
    context_state_callback,
    source
	);

    int ret = 0;

    if (pa_mainloop_run(m_pulseaudio_mainloop, &ret) < 0) {
        fprintf(stderr, "PulseAudio mainloop failed\n");
        pa_context_disconnect(context);
        pa_context_unref(context);
        pa_mainloop_free(m_pulseaudio_mainloop);

        return 1;
    }

	pa_buffer_attr pb = {.maxlength = (uint32_t)-1, // BUFSIZE * 2,
                         .fragsize = buffer_size};



/*sample specs*/
	static const pa_sample_spec ss = {
	.format = PA_SAMPLE_S16LE, // 16 bit signed integers
	.rate = 44100, // hertz per second per channel
	.channels = 2 // L,R for stereo audio
	};
	pa_simple *s = NULL;
	int error;


    s = pa_simple_new(
        NULL,                   /* default PulseAudio server */
        "basstap",              /* application name */
        PA_STREAM_RECORD,       /* recording */
        source,                 /* .monitor source */
        "audio for basstap",    /* stream name */
        &ss,                    /* sample specification */
        NULL,                   /* default channel map */
        &pb,                    /* buffer attributes */
        &error
    );


    if (s == NULL) {
        fprintf(
            stderr,
            "pa_simple_new() failed: %s\n",
            pa_strerror(error)
        );

        pa_mainloop_free(m_pulseaudio_mainloop);

        return 1;
    }


    printf("Successfully connected to audio source!\n");


    while (1) {

        if (pa_simple_read(
                s,
                buf,
                buffer_size,
                &error
            ) < 0) {

            fprintf(
                stderr,
                "pa_simple_read() failed: %s\n",
                pa_strerror(error)
            );

            break;
        }

        printf("Received %zu bytes\n", buffer_size);
    }

    pa_simple_free(s);

    pa_mainloop_free(m_pulseaudio_mainloop);

    return 0;




}

