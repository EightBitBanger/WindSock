//
// HTTP response codes

namespace STATECODE {
    // General
    const char ok[]               = "200 OK";
    const char not_found[]        = "404 Not found";
    const char processing[]       = "102 Processing";
    const char no_content[]       = "204 No Content";
    // Errors
    const char bad_request[]      = "400 Bad Request";
    const char unauthorized[]     = "401 Unauthorized";
    const char forbidden[]        = "403 Forbidden";
    // Internal errors
    const char server_error[]     = "500 Internal Server Error";
    const char bad_gateway[]      = "502 Bad Gateway";
    const char gateway_timeout[]  = "504 Gateway Timeout";
    const char unavailable[]      = "503 Service Unavailable";
}


//
// HTTP content formats

namespace CONTENTTYPE {
    // Text
    const char text_plain[]    = "text/plain";
    const char text_html[]     = "text/html";
    const char text_xml[]      = "text/xml";
    const char text_rtf[]      = "text/rtf";
    const char text_css[]      = "text/css";
    const char text_csv[]      = "text/csv";
    const char text_csvsch[]   = "text/csv-schema";
    const char text_md[]       = "text/markdown";
    const char text_js[]       = "text/javascript";
    // Images
    const char image_jpeg[]    = "image/jpeg";
    const char image_png[]     = "image/png";
    const char image_bmp[]     = "image/bmp";
    const char image_svgxml[]  = "image/svg+xml";
    // Video
    const char video_mp4[]     = "video/mp4";
    const char video_ogg[]     = "video/ogg";
    // Audio
    const char audio_ogg[]     = "audio/ogg";
    // Model
    const char model_obj[]     = "model/obj";
    const char model_mtl[]     = "model/mtl";
    // Application
    const char octet_stream[]  = "application/octet-stream";
}


//
// Connection  states

namespace CONNECTION {
    // Close connection after initial communication
    const char close[]         = "close";
    // Keep connection open after initial communication
    const char keep_alive[]    = "keep-alive";
}

