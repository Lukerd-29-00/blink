#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>


int main(void){
    AVFormatContext *pFormatCtx = NULL;
    if(avformat_open_input(&pFormatCtx,"input/Blink_1.avi",NULL,NULL))
        return -1;
    
    int err = avformat_find_stream_info(pFormatCtx,NULL);
    if(err<0)
        return err;
    int stream;
    for(stream = 0;stream < pFormatCtx->nb_streams;stream++){
        if(pFormatCtx->streams[stream]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            break;
        }
    }
    AVCodecParameters *pCodecPar = pFormatCtx->streams[stream]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    if(pCodec == NULL){
        return -1;
    }
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);

    AVFrame *frame = av_frame_alloc();
    AVFrame *frameRGB = av_frame_alloc();


    avcodec_parameters_to_context(pCodecCtx,pCodecPar);

    int size = avpicture_get_size(AV_PIX_FMT_RGB24,pCodecCtx->width,pCodecCtx->height);

    uint8_t *buffer = av_malloc(sizeof(uint8_t)*size);

    avpicture_fill(frameRGB,buffer,AV_PIX_FMT_RGB24,pCodecCtx->width,pCodecCtx->height );

    AVDictionary *options = NULL;    
    if(avcodec_open2(pCodecCtx,pCodec,&options) < 0)
        return -1;

    AVPacket packet;

    av_read_frame(pFormatCtx,&packet);

    avcodec_send_packet(pCodecCtx,&packet);

    avcodec_receive_frame(pCodecCtx,frame);

    struct SwsContext *swsCtx = sws_getContext(pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_RGB32,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    sws_scale(swsCtx, frame->data, frame->linesize, 0, pCodecCtx->height, frameRGB->data,frameRGB->linesize);

    printf("%d\n",frame->height);
    printf("%d\n",frame->width);
    printf("%d\n",frame->format);

    for(int i = 0; i < 3;i++){
        printf("%hhd\n",frameRGB->data[0][i]);
    }
}