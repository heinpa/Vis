#include <GLApp.h>
#include <FontRenderer.h>

class MyGLApp : public GLApp {
public:
  Image image{640,480};
  FontRenderer fr{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fe{nullptr};
  std::string text;

  MyGLApp() : GLApp{800,800,1,"Color Picker"} {}
  
  Vec3 convertPosFromHSVToRGB(float x, float y) {
    float hue = x*360;
    float saturation = y;
    float value = 1.0f;
    float red, green, blue;
    // find chroma
    float chroma = value * saturation;
    // find a point along the bottom 3 faces of the RGB cube with same hue and chroma
    float huePrime = fmod(hue / 60.0, 6);
    // calculate the second largest component of the colour
    float secondary = chroma * (1 - fabs(fmod(huePrime, 2) - 1));

    if(0 <= huePrime && huePrime < 1) {
      red = chroma;
      green = secondary;
      blue = 0;
    } else if(1 <= huePrime && huePrime < 2) {
      red = secondary;
      green = chroma;
      blue = 0;
    } else if(2 <= huePrime && huePrime < 3) {
      red = 0;
      green = chroma;
      blue = secondary;
    } else if(3 <= huePrime && huePrime < 4) {
      red = 0;
      green = secondary;
      blue = chroma;
    } else if(4 <= huePrime && huePrime < 5) {
      red = secondary;
      green = 0;
      blue = chroma;
    } else if(5 <= huePrime && huePrime < 6) {
      red = chroma;
      green = 0;
      blue = secondary;
    } else {
      red = 0;
      green = 0;
      blue = 0;
    }

    // adjust final RGB results to match value
    float m = value - chroma;
    red += m;
    green += m;
    blue += m;

    return Vec3{red,green,blue};
  }
  
  virtual void init() override {
    fe = fr.generateFontEngine();
    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb = convertPosFromHSVToRGB(float(x)/image.width, float(y)/image.height);
        image.setNormalizedValue(x,y,0,rgb.r); image.setNormalizedValue(x,y,1,rgb.g);
        image.setNormalizedValue(x,y,2,rgb.b); image.setValue(x,y,3,255);
      }
    }
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
  }
  
  virtual void mouseMove(double xPosition, double yPosition) override {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;
    const Vec3 hsv{float(360*xPosition/s.width),float(1.0-yPosition/s.height),1.0f};
    const Vec3 rgb = convertPosFromHSVToRGB(float(xPosition/s.width), float(1.0-yPosition/s.height));
    std::stringstream ss; ss << "HSV: " << hsv << "  RGB: " << rgb; text = ss.str();
  }
    
  virtual void draw() override {
    drawImage(image);

    const Dimensions dim{ glEnv.getFramebufferSize() };
    fe->render(text, dim.aspect(), 0.03f, {0,-0.9f}, Alignment::Center, {0,0,0,1});
  }
};

#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
    MyGLApp myApp;
    myApp.run();
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
