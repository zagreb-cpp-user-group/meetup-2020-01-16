#include "FaceDetector.hpp"

#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

namespace
{
    template< typename Outcome >
    auto succeedOrDie( Outcome && outcome )
    {
        if ( outcome.has_value() )
        {
            return std::forward< Outcome >( outcome ).value();
        }
        else
        {
            EM_ASM_
            (
                {
                    throw new Error( UTF8ToString( $0 ) );
                },
                outcome.error().data()
            );
            // silence warning: control may reach end of non-void function
            return std::forward< Outcome >( outcome ).value();
        }
    }
}

struct EmscriptenFaceDetection
{
    bool detected = false;
    int  x        = 0;
    int  y        = 0;
    int  width    = 0;
    int  height   = 0;
};

class EmscriptenFaceDetector
{
public:
    EmscriptenFaceDetector() :
        detector_{ succeedOrDie( createFaceDetector() ) }
    {}

    EmscriptenFaceDetection detectFaces( emscripten::val const & jsImageData )
    {
        obtainImage( jsImageData );
        auto const & faces = detector_.detectFaces( rgbaImage_ );
        if ( faces.empty() )
        {
            return {};
        }
        else
        {
            auto const & firstFace = faces[ 0 ];
            return { true, firstFace.x, firstFace.y, firstFace.width, firstFace.height };
        }
    }

private:
    FaceDetector detector_;
    cv::Mat rgbaImage_;

    void obtainImage( emscripten::val const & jsImageData )
    {
        auto width = jsImageData[ "width" ].as< unsigned long >();
        auto height = jsImageData[ "height" ].as< unsigned long >();
        rgbaImage_.create( height, width, CV_8UC4 );
        auto imgData = jsImageData[ "data" ].as< emscripten::val >();

        // taken from https://sean.voisen.org/blog/2018/03/rendering-images-emscripten-wasm/
        auto length = imgData[ "length" ].as< unsigned int >();
        emscripten::val memory = emscripten::val::module_property( "buffer" );

        std::uint8_t * destBuffer = rgbaImage_.data;
        emscripten::val memoryView( emscripten::typed_memory_view( length, destBuffer ) );
        memoryView.call< void >( "set", imgData );
    }
};

using namespace emscripten;
EMSCRIPTEN_BINDINGS( FaceDetector )
{
    class_< EmscriptenFaceDetector >( "FaceDetector" )
        .constructor()
        .function( "detectFaces", &EmscriptenFaceDetector::detectFaces );

    value_object< EmscriptenFaceDetection >( "FaceDetection" )
        .field( "detected", &EmscriptenFaceDetection::detected )
        .field( "x"       , &EmscriptenFaceDetection::x        )
        .field( "y"       , &EmscriptenFaceDetection::y        )
        .field( "width"   , &EmscriptenFaceDetection::width    )
        .field( "height"  , &EmscriptenFaceDetection::height   );
}
