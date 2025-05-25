#include <cstdint>
#include <iomanip>
#include <fstream>

#include <GLApp.h>
#include <bmp.h>
#include <Grid2D.h>
#include <stdint.h>


class GLIPApp : public GLApp {
public:
  Image image;
	
  GLIPApp() : GLApp(512, 512, 4, "Image Processing")
  {
  }
  
  void toGrayscale(bool uniform=false) {
	float r, g, b;
	float c_lin;

	for (uint32_t y = 0;y<image.height;++y) {
		for (uint32_t x = 0;x<image.width;++x) {

		r = (float)image.getValue(x, y, 0)/255;
		g = (float)image.getValue(x, y, 1)/255;
		b = (float)image.getValue(x, y, 2)/255;

		if (uniform) {
			c_lin = r/3.0 + b/3.0 + g/3.0;
		} else {
			c_lin = r * 0.299 + g * 0.587 + b * 0.114;
		}
		image.setNormalizedValue(x,y,c_lin);
		}
	}
  }

  void loadImage() {
	try {
		image = BMP::load("lenna.bmp");
	} catch (...) {
		image = Image(512,512);
		for (uint32_t y = 0;y<image.height;++y) {
		for (uint32_t x = 0;x<image.width;++x) {
			image.setNormalizedValue(x,y,0,float(x)/image.width);
			image.setNormalizedValue(x,y,1,float(y)/image.height);
			image.setNormalizedValue(x,y,2,0.5f);
			image.setValue(x,y,3,255);
		}
		}
	}
  }
  
  virtual void init() override {
	loadImage();
  }
		
  virtual void draw() override {
	drawImage(image);
  }
  
  
  std::string toString(bool bSmallTable=true) {
	const std::string lut1{"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "};
	const std::string lut2{"@%#*+=-:. "};
	const std::string& lut = bSmallTable ? lut2 : lut1;
	
	std::stringstream ss;
	for (uint32_t i=image.height; i>0; i-=2) {
		for (uint32_t j=0; j<image.width; j+=2) {
			uint8_t idx = (255 - image.getLumiValue(j, i-1)) * lut.size() / 256;
			// append the character twice, as it was done for the reference image
			ss << lut[idx] << lut[idx];
		}
		ss << "\n";
	}
	return ss.str();
  }

  void filter(const Grid2D& filter) {

	Image output = Image(image.width, image.height);

	for (uint8_t channel=0; channel<3; channel++) {
		for (uint32_t i=0; i<output.height; i++) {
			for (uint32_t j=0; j<output.width; j++) {
				float c = 0.0f;
				uint8_t hh = filter.getHeight() / 2;
				uint8_t ww= filter.getWidth() / 2;
				for (uint32_t h=0; h<filter.getHeight(); h++) {
					for (uint32_t w=0; w<filter.getWidth(); w++) {
						int x = i - hh + h;
						int y = j - ww + w;
						if (x >= 0 && x < (int)image.height && y >= 0 && y < (int)image.width) {
							c += (float)filter.getValue(w, h) * image.getValue(y, x, channel);
						}
					}
				}
				output.setValue(j, i, channel, fabs(c));
			}
		}
	}
	image = output;
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override {
	if (action == GLENV_PRESS) {
		switch (key) {
		case GLENV_KEY_ESCAPE :
			closeWindow();
			break;
		case GLENV_KEY_M :
			{
			Grid2D mean{3,3};
			mean.fill(1.0f/(mean.getHeight()*mean.getWidth()));
			filter(mean);
			}
			break;
		case GLENV_KEY_A :
			filter({3,3, {-1, 0, 1,
						-2, 0, 2,
						-1, 0, 1}});
			break;
		case GLENV_KEY_B:
			filter({3,3, {-1,-2,-1,
						 0, 0, 0,
						 1, 2, 1}});
		break;
		case GLENV_KEY_G :
			toGrayscale(false);
			break;
		case GLENV_KEY_H :
			toGrayscale(true);
			break;
		case GLENV_KEY_R :
			loadImage();
			break;
		case GLENV_KEY_T : {
			std::ofstream file{ "ascii.txt" };
			file << toString() << std::endl;
			break;
		}
		}
	}
  }
  
};

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
	GLIPApp imageProcessing;
	imageProcessing.run();
  }
  catch (const GLException& e) {
	std::stringstream ss;
	ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
	std::cerr << ss.str().c_str() << std::endl;
#else
	MessageBoxA(
		NULL,
		ss.str().c_str(),
		"OpenGL Error",
		MB_ICONERROR | MB_OK
	);
#endif
	return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
