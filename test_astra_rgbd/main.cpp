#include <vector>

#include <astra_core/astra_core.hpp>
#include <astra/astra.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

astra::InfraredStream configure_ir(astra::StreamReader& reader, bool useRGB)
{
	auto irStream = reader.stream<astra::InfraredStream>();

	auto oldMode = irStream.mode();

	astra::ImageStreamMode irMode;
	irMode.set_width(640);
	irMode.set_height(480);

	if (useRGB)
	{
		irMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_RGB888);
	}
	else
	{
		irMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_GRAY16);
	}

	irMode.set_fps(30);
	irStream.set_mode(irMode);

	auto newMode = irStream.mode();
	printf("Changed IR mode: %dx%d @ %d -> %dx%d @ %d\n",
		oldMode.width(), oldMode.height(), oldMode.fps(),
		newMode.width(), newMode.height(), newMode.fps());

	return irStream;
}

astra::ColorStream configure_color(astra::StreamReader& reader)
{
	auto colorStream = reader.stream<astra::ColorStream>();

	auto oldMode = colorStream.mode();

	astra::ImageStreamMode colorMode;
	colorMode.set_width(640);
	colorMode.set_height(480);
	colorMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_RGB888);
	colorMode.set_fps(30);

	colorStream.set_mode(colorMode);

	auto newMode = colorStream.mode();
	printf("Changed color mode: %dx%d @ %d -> %dx%d @ %d\n",
		oldMode.width(), oldMode.height(), oldMode.fps(),
		newMode.width(), newMode.height(), newMode.fps());

	return colorStream;
}

astra::DepthStream configure_depth(astra::StreamReader& reader)
{
	auto depthStream = reader.stream<astra::DepthStream>();

	auto oldMode = depthStream.mode();

	//We don't have to set the mode to start the stream, but if you want to here is how:
	astra::ImageStreamMode depthMode;

	depthMode.set_width(640);
	depthMode.set_height(480);
	depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
	depthMode.set_fps(30);

	depthStream.set_mode(depthMode);

	auto newMode = depthStream.mode();
	printf("Changed depth mode: %dx%d @ %d -> %dx%d @ %d\n",
		oldMode.width(), oldMode.height(), oldMode.fps(),
		newMode.width(), newMode.height(), newMode.fps());

	return depthStream;
}

std::vector<cv::Point> capturePoint;

static void calibMouseCallback(int event, int x, int y, int flags, void* image) {

	cv::Mat timg;
	cv::Mat img = *(cv::Mat*)image;
	img.copyTo(timg);

	cv::Point pt;

	if (event == cv::EVENT_MOUSEMOVE) {
		cv::drawMarker(timg, cv::Point2d(x, y), cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 50, 1);
		cv::imshow("img", timg);
	}
	if (event == cv::EVENT_LBUTTONDOWN) {
		pt = cv::Point(x, y);
		capturePoint.emplace_back(pt);

		cv::drawMarker(img, cv::Point2d(x, y),cv::Scalar(0,0,255), cv::MARKER_CROSS, 20, 1);
		std::cout << "x , y : " << x << " " << y << std::endl;
	}
}

int main() {

#if 0
	astra::initialize();

	astra::StreamSet streamSet;
	astra::StreamReader reader = streamSet.create_reader();

	reader.stream<astra::PointStream>().start();

	auto depthStream = configure_depth(reader);
	depthStream.start();

	auto colorStream = configure_color(reader);
	colorStream.start();

	auto irStream = configure_ir(reader, false);
	int key_pressed = -1;
	while (key_pressed != 'q') {
		astra_update();

		auto astra_color_frame = reader.get_latest_frame().get<astra::ColorFrame>();
		auto astra_depth_frame = reader.get_latest_frame().get<astra::DepthFrame>();

		cv::Mat color = cv::Mat(480, 640, CV_8UC3);
		cv::Mat depth = cv::Mat(480, 640, CV_16SC1);

		std::memcpy(color.data, (void*)astra_color_frame.data(), 480 * 640 * 3);
		std::memcpy(depth.data, (void*)astra_depth_frame.data(), 480 * 640 * 2);

		cv::cvtColor(color, color, cv::COLOR_BGR2RGB);
		cv::flip(color, color, 1);

		cv::imshow("color", color);
		cv::imshow("depth", depth);
		
		key_pressed = cv::waitKey(1);
		if (key_pressed == 'c') {
			cv::imwrite("color.png", color);
			break;
		}
	}
#endif
	//cv::Mat img = cv::imread("color.png");

	//cv::namedWindow("img", 1);
	//cv::setMouseCallback("img", calibMouseCallback, (void*)&img);
	//cv::waitKey(0);

	//for (auto x : capturePoint) {
	//	std::cout << x.x << " " << x.y << std::endl;
	//}

	cv::VideoCapture cam;
	//cam.set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
	cam.open(0);
	if (!cam.isOpened()) {
		std::cout << "[Error] webcam stream is not available" << std::endl;
		return false;
	}
	cam.set(cv::CAP_PROP_EXPOSURE, 0);
	while (true) {
		cv::Mat frame;
		cam >> frame;

		cv::imshow("frame", frame);
		cv::waitKey(1);
	}
	
	


	return 0;
}