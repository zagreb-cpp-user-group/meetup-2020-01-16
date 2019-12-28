#include "FaceDetector.hpp"

#include <iostream>

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
            std::cerr << outcome.error() << std::endl;
            std::terminate();
        }
    }

    auto drawFaces( cv::Mat & image, std::vector< cv::Rect > const & rectangles )
    {
        static auto const color = cv::Scalar{ 0.f, 0.f, 255.f };
        for ( auto && rect : rectangles )
        {
            cv::rectangle( image, { rect.x, rect.y }, { rect.x + rect.width, rect.y + rect.height }, color );
        }
    }

    static constexpr auto ESC_KEYCODE = 27;
}

int main()
{
    return 0;
}
