// Compile with g++ $(pkg-config --cflags --libs opencv)
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
// #include <opencv2/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cmath>
// #include <time.h>
#include <ctime>

#include <nan.h>
#include <node.h>

using namespace std;




#define LSBUINT32COLOR(x,y,z) ( (0xFF << 24) | ((z) << 16) | ((y) << 8) | (x) )
#define MSBUINT32COLOR(x,y,z) ( ((x) << 24) | ((y) << 16) | ((z) << 8) | 0xFF )
#define UNPACKLSBUINT32COLOR(x,y,z,u32c) { (x) = (u32c & 0xFF);      (y) = (u32c >>  8) &0xFF; (z) = (u32c >> 16) &0xFF;  }
#define UNPACKMSBUINT32COLOR(x,y,z,u32c) { (x) = (u32c >> 24)& 0xFF; (y) = (u32c >> 16) &0xFF; (z) = (u32c >>  8) &0xFF;  }

const uint_fast8_t MAX8  = 0xFF;
const uint_fast16_t MAX16 = 0xFFFF;
const uint_fast32_t MAX32 = 0xFFFFFFFF;


typedef struct Options {
  char effect;
  size_t order;
  size_t height;
  size_t width;
  size_t length;
  uint_least8_t channel;
  uint_least8_t threshold;
  double depth;
  uint_least8_t xspace;
  uint_least8_t yspace;
  uint_least8_t stroke;
  uint_least8_t weight;
  uint_least32_t color;
} Options;
  



int isLSB() 
{
  union {
    long int l;
    char c[sizeof (long int)];
  } u;

  u.l = 1;

  if (u.c[sizeof(long int)-1] == 1) {
    return 0;
  } else {
    return 1;
  }
}


// void readImageDataFromOpenCv (std::string filename, uint_least8_t *cvpixels, Options *opts);
// void TestArrays(cv::Mat& I);
void setOptions(const cv::Mat & m, Options *opts);
void A8Bit2BcopyBuffer(uint_fast8_t *chunk, uint_fast32_t *dupBuffer, Options *opts);
void A2B8BitcopyBuffer(uint_fast32_t *chunk, uint_fast8_t *dupBuffer, Options *opts);
void A2BcopyBuffer(uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts);
int channelSelector32LSB(uint_least32_t *chunk, uint_least32_t *dupBuffer, Options *opts);

// noop
void noop  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// poisson blends
void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void source_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void source_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void source_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void sample_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void poisson_blend_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);


void poisson_blend_add_sample_void(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_subtract_sample_void(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_add_source_void(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_subtract_source_void(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// better math
void poisson_blend_add_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_subtract_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_multiply_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void poisson_blend_divide_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// pixelsorts
void pixelsort (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// displacements
void deplace  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// reverbs
void wet_reverb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// zooms
void zoomth (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void zoom_b (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void zoom_r (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

void crunch (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// colorswap
void rgb_2_bgr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_gbr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_grb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void rgb_2_brg (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// shapes

void pixelate (uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts);

void outtashape (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtacircle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtatriangle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void outtasquare (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// copy shapes - these can be used for selections
void copyBlocks (uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh);
void copyCircles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh, size_t radius);
void copyTriangles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh);

// Data moshes
void smosh (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void smosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void posht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void fosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void losht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// gradient blends
void gradient_blend_add_sample(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_add_source(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_subtract_sample(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
void gradient_blend_subtract_source(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// test
void pssht(uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);

// void pixelswap (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts);
// void convertCVDataTo32bit(uint_least8_t *cvPixels, uint_fast32_t* rgbPixels , uint_fast32_t *dupBuffer,size_t size);
void convertCVDataTo32bit(uint_least8_t *cvPixels, uint_least32_t* rgbPixels , size_t size);
void convert32bitToCVData(uint_least8_t *cvPixels, uint_least32_t* rgbPixels , size_t size, size_t length);

void funnelEffect(std::uint_fast32_t *pixels, std::uint_fast32_t *clone, Options *opts );



void getParametersFromNode(v8::Local<v8::Array>, Options*);



class MyWorker : public Nan::AsyncWorker {
 public:
  // Constructor
  MyWorker(Nan::Callback *callback,std::string filename, std::string outfile, Options *stack, size_t stacksize)
    : Nan::AsyncWorker(callback), filename(filename), outfile(outfile), stack(stack), stacksize(stacksize) {}
    // : Nan::AsyncWorker(callback), delay(delay) {}
  // Destructor
  ~MyWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    

    // Asynchronous, non-V8 work goes here
    std::cout << "Reading from \"The stack\", Asynchronously " << std::endl;
    for (size_t i = 0; i < this->stacksize; ++i) {
      std::cout<< "-------------------------------" << std::endl;
      std::cout<< "Reading from stack position #" << i << std::endl;
      std::cout<< "-------------------------------" << std::endl;
      std::cout << "Effect: " << this->stack[i].effect << std::endl;
      std::cout << "Channel: " << (std::uint_least32_t)this->stack[i].channel << std::endl;
      std::cout << "Threshold: " << this->stack[i].threshold << std::endl;
      std::cout << "Depth: " << this->stack[i].depth << std::endl;
      std::cout << "X-Space: " << (std::uint_least32_t)this->stack[i].xspace << std::endl;
      std::cout << "Y-Space: " << (std::uint_least32_t)this->stack[i].yspace << std::endl;
      std::cout << "Stroke: " << (std::uint_least32_t)this->stack[i].stroke << std::endl;
      std::cout << "Weight: " << (std::uint_least32_t)this->stack[i].weight << std::endl;
      std::cout << "Color: " << this->stack[i].color << std::endl;

    }

    // timing
    // double t;
    // t = (double)cv::getTickCount();
    
    // http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    // time_t now = time(0);
    // tm * thetimelocal = localtime(&now);
    // std:: string RandomNumberString = asctime(thetimelocal);
    // int phase = 0;

    // Init phase
    // phase = 1;

    // OBTAIN FILENAME!
    // std::string filname;

    std::cout << "phase 1" << std::endl;
    // std::size_t num;
    uint_least8_t * pixels;
    cv::Mat m;
    // filname = argv[1];
    // std::istringstream iss(argv[2]);
    // if (!(iss >> num)) std::cerr << "Invalid number " << argv[2] << std::endl;
    
    std::cout << "phase 2" << std::endl;
    // std::cout << "Image file is " << this->filename << std::endl;
    // m = cv::imread("./Nicole_Lee_Widga.jpg", CV_LOAD_IMAGE_COLOR);
    m = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    
    std::cout << "phase 3" << std::endl;
    if (!m.data) return;
    size_t elemSize = m.elemSize();
    size_t total = m.total();
    size_t length = elemSize * total;
    // std::cout << "length" << length << endl;

    // Set pixels to point at the data in the cv::Mat
    pixels = m.data;
    // Set each stack's size off of the image size
    for (size_t n = 0; n < this->stacksize; ++n) {
      setOptions(m, &this->stack[n]);
      
    }

    std::cout << "phase 4" << std::endl;
    // readImageDataFromOpenCv(filename, pixels, &opts)
    
    uint_fast32_t * rgbPixels = new uint_fast32_t [ length ];
    uint_fast32_t * dupPixels = new uint_fast32_t [ length ];
    
    
    convertCVDataTo32bit(pixels, rgbPixels, length);
    A2BcopyBuffer(rgbPixels, dupPixels,  &this->stack[0]);
    
    /*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to init. " << endl;
    t = cv::getTickCount();
    */
      

    // Set up phase
    // phase = 2;
    /*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to setup. " << endl;
    t = cv::getTickCount();
    */



    // Run phase
    // phase = 3;
    std::cout << "will run " << this->stacksize << " times" << std::endl;
    for (size_t i = 0; i < this->stacksize; ++i) {

    /*

      for video, do something like
      size_t effect_start , effect_end
      effect_start = stack[i].effect_start;
      effect_end   = stack[i].effect_end;
      if (effect_start != findcurrentframetime() ) {
        continue;
      } 
      else if (effect_end != findcurrentframetime()) {
        continue;
      }

    */

      std::cout << "Run #" << i << "." << std::endl;
      std::cout << "Effect is " << (char)this->stack[i].effect << "." << std::endl;


      switch(stack[i].order) {
        case 0 :
          funnelEffect(rgbPixels, dupPixels, &this->stack[i]);
          break;
        case 1 :
          funnelEffect(dupPixels, rgbPixels, &this->stack[i]);
          break;
        case 2 :
          funnelEffect(rgbPixels, rgbPixels, &this->stack[i]);
          break;
        case 3 :
          funnelEffect(dupPixels, dupPixels, &this->stack[i]);
          break;
        default:
          funnelEffect(rgbPixels, dupPixels, &this->stack[i]); 
          break;
      }

    }


    /*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to finish all effects. " << endl;
    t = cv::getTickCount();
    */

    // Write phase
    // phase = 4;
    std::cout << "phase 5" << std::endl;

    A2BcopyBuffer(dupPixels, rgbPixels,  &this->stack[0]);
    convert32bitToCVData(pixels, rgbPixels , total, length);
    
    // Output and output type here
    // string RandomNumberString = static_cast<ostringstream*>( &(ostringstream() << t ) )->str();
    std::cout << "phase 6" << std::endl;
   
    cv::imwrite( outfile, m);
    // cv::imwrite(  "Test.jpg", m);

    std::cout << "phase 7" << std::endl;

/*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to writing file(s). " << endl;
    t = cv::getTickCount();
*/


  // Clean up phase
    // phase = 5;
    // delete[] &this->stack;
    std::cout << "delete dupPixels" << std::endl;
    // delete[] dupPixels;
    std::cout << "delete rgbPixels" << std::endl;
    // delete[] rgbPixels;
/*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to clean up. " << endl;
    */


  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::HandleScope scope;

    // Nan::Callback#Call() does a Nan::MakeCallback() for us
    // This is where you change where stuff gets called
    callback->Call(0, NULL);
  }

 private:
  std::string filename;
  std::string outfile;
  Options *stack;
  std::size_t stacksize;
  // Nan::Callback *callback;
};





/* ===================================================
    Buffer version

   ===================================================
*/ 



class MyBufferWorker : public Nan::AsyncWorker {
 public:
  // Constructor
  MyBufferWorker(Nan::Callback *callback, uint_fast8_t *pixels, Options *stack, std::size_t stacksize, std::size_t bufferlength)
    : Nan::AsyncWorker(callback), pixels(pixels), stack(stack), stacksize(stacksize), bufferlength(bufferlength) {}
 
  // Destructor
  ~MyBufferWorker() {}

  void Execute () {
    

    // Asynchronous, non-V8 work goes here
    // std::cout << "Reading from \"The stack\", Asynchronously " << std::endl;
    for (size_t i = 0; i < this->stacksize; ++i) {
      std::cout<< "-------------------------------" << std::endl;
      std::cout<< "Reading from stack position #" << i << std::endl;
      std::cout<< "-------------------------------" << std::endl;
      std::cout << "Length: " << this->stack[i].length << std::endl;
      std::cout << "Width: " << this->stack[i].width << std::endl;
      std::cout << "Height: " << this->stack[i].height << std::endl;
      std::cout << "Effect: " << (std::uint_least32_t)this->stack[i].effect << std::endl;
      std::cout << "Order: " << (std::uint_least32_t)this->stack[i].order << std::endl;
      std::cout << "Channel: " << (std::uint_least32_t)this->stack[i].channel << std::endl;
      std::cout << "Threshold: " << (std::uint_least32_t)this->stack[i].threshold << std::endl;
      std::cout << "Depth: " << this->stack[i].depth << std::endl;
      std::cout << "X-Space: " << (std::uint_least32_t)this->stack[i].xspace << std::endl;
      std::cout << "Y-Space: " << (std::uint_least32_t)this->stack[i].yspace << std::endl;
      std::cout << "Stroke: " << (std::uint_least32_t)this->stack[i].stroke << std::endl;
      std::cout << "Weight: " << (std::uint_least32_t)this->stack[i].weight << std::endl;
      std::cout << "Color: " << this->stack[i].color << std::endl;
      std::cout<< "-------------------------------" << std::endl;
      std::cout<< "End read from stack position #" << i << std::endl;
      std::cout<< "-------------------------------" << std::endl;
    }


    std::cout << "phase 1" << std::endl;
    std::cout << "phase 2" << std::endl;
    std::cout << "phase 3" << std::endl;
    // Make extra doubly sure you're passing in the w/h/length on ALL of the objects!

    // Set each stack's size off of the image size
    // for (size_t n = 0; n < this->stacksize; ++n) {
    //   setOptions(m, &this->stack[n]);
      
    // }


    std::cout << "phase 4" << std::endl;
    // readImageDataFromOpenCv(filename, pixels, &opts)
    // uint_fast32_t * rgbPixels = reinterpret_cast<uint_fast32_t*>(this->pixels);

    // Buffer is larger to account for errors in maths :p
    uint_fast32_t * rgbPixels = new uint_fast32_t [ this->bufferlength ];
    uint_fast32_t * dupPixels = new uint_fast32_t [ this->bufferlength ];

    // uint_fast32_t * rgbPixels = new uint_fast32_t [ this->stack[0].length ];
    // std::vector<uint_fast32_t> rgbPixels(length);
    // std::vector<uint_fast32_t> rgbPixels(length);
    // convertCVDataTo32bit(pixels, rgbPixels, length);
    // convert8BitCanvasTo32Bit(this->pixels, rgbPixels, this->stack[0].length);
    std::cout<< "phase 4.5" << std::endl;
    A8Bit2BcopyBuffer(pixels, rgbPixels,  &this->stack[0]);
    std::cout<< "phase 4.75" << std::endl;
    A2BcopyBuffer(rgbPixels, dupPixels,  &this->stack[0]);

    std::cout << "will run " << this->stacksize << " times" << std::endl;
    for (size_t i = 0; i < this->stacksize; ++i) {

      std::cout << "Run #" << i << "." << std::endl;
      std::cout << "Effect is " << (char)this->stack[i].effect << "." << std::endl;


      switch(stack[i].order) {
        case 0 :
          funnelEffect(rgbPixels, dupPixels, &this->stack[i]);
          break;
        case 1 :
          funnelEffect(dupPixels, rgbPixels, &this->stack[i]);
          break;
        case 2 :
          funnelEffect(rgbPixels, rgbPixels, &this->stack[i]);
          break;
        case 3 :
          funnelEffect(dupPixels, dupPixels, &this->stack[i]);
          break;
        default:
          funnelEffect(rgbPixels, dupPixels, &this->stack[i]); 
          break;
      }

    }


    /*
    t = 1000*((double)cv::getTickCount() - t)/cv::getTickFrequency();
    cout << "Took " << t << " milliseconds to finish all effects. " << endl;
    t = cv::getTickCount();
    */

    // Write phase
    // phase = 4;
    std::cout << "phase 5" << std::endl;

    A2BcopyBuffer(dupPixels, rgbPixels,  &this->stack[0]);
    std::cout << "phase 5.5" << std::endl;
    std::cout << "stack   length" << this->stack[0].length << std::endl;
    std::cout << "orginal length" << this->bufferlength << std::endl;
    A2B8BitcopyBuffer(rgbPixels, this->pixels, &this->stack[0]);
    // this->pixels = reinterpret_cast<uint_fast8_t*>(rgbPixels);
    // convert32bitToCVData(pixels, rgbPixels , total, this->stack[0].length);
    // Output and output type here
    // string RandomNumberString = static_cast<ostringstream*>( &(ostringstream() << t ) )->str();
    std::cout << "phase 6" << std::endl;
   
    // cv::imwrite( outfile, m);
    // cv::imwrite(  "Test.jpg", m);

    std::cout << "phase 7" << std::endl;


  // Clean up phase
    // phase = 5;
    // delete[] &this->stack;
    std::cout << "delete dupPixels" << std::endl;
    // delete[] dupPixels;
    std::cout << "delete rgbPixels" << std::endl;
    // delete[] rgbPixels;

  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::HandleScope scope;
    // Nan::Callback#Call() does a Nan::MakeCallback() for us
    // This is where you change where stuff gets called

    // Do this work here
    // v8::Local<v8::Object> buf = Nan::NewBuffer(this->stack[0].length).ToLocalChecked();
    // char* pbuf = node::Buffer::Data(buf);
    char * nbuf = reinterpret_cast<char*>(this->pixels);
    std::cout << "right before remaking buffer " << std::endl;
    v8::Local<v8::Object> buf  = Nan::NewBuffer(nbuf, this->bufferlength).ToLocalChecked();
    std::cout << "after making new buffer" << std::endl;

    v8::Local<v8::Value> argv[] = {
      buf
    };

    std::cout << "Finished" << std::endl;
    callback->Call(1, argv);

  }

 private:
  uint_fast8_t *pixels;
  Options *stack;
  std::size_t stacksize;
  std::size_t bufferlength;
  // Nan::Callback *callback;
};







NAN_METHOD(UseFilename) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> val;
  v8::Local<v8::Object> instructions;
  v8::Local<v8::Number> num;
  v8::Local<v8::Integer> integer;

  v8::Local<v8::String> filenamestring = info[0].As<v8::String>();
  if (filenamestring.IsEmpty()) {
    Nan::SyntaxError("Must pass file name as first argument <SyntaxError>");
  }
  std::string imagefile = *v8::String::Utf8Value(filenamestring);

  v8::Local<v8::String> outfilenamestring = info[1].As<v8::String>();
  if (outfilenamestring.IsEmpty()) {
    Nan::SyntaxError("Must pass out file name as second argument <SyntaxError>");
  }
  std::string outfile = *v8::String::Utf8Value(outfilenamestring);

  // Options Stack
  v8::Local<v8::Array> fullGStack = info[2].As<v8::Array>();
  
  // Canvas Data
  // Nan::TypedArrayContents<uint8_t> data(info[1]);
  
  // Callback Data
  v8::Local<v8::Function> hollerback = info[3].As<v8::Function>();


  std::size_t runTimes = fullGStack->Length();
  // initialize el stacko...mmm..steak tacos
  Options *stack = new Options[runTimes];

  getParametersFromNode(fullGStack, stack);


 
  Nan::Callback* holla = new Nan::Callback(hollerback);
  std::cout << imagefile << std::endl;
  MyWorker *worker = new MyWorker(holla, imagefile, outfile, stack, runTimes);
  Nan::AsyncQueueWorker(worker);  
  // Nan::ThrowError("Jus cause cuz");
  info.GetReturnValue().Set(Nan::New("ok!").ToLocalChecked());
}





NAN_METHOD(UseBuffer) {
  Nan::HandleScope scope;
  v8::Local<v8::Integer> integer;
  // needed
  v8::Local<v8::Value> val;
  v8::Local<v8::Object> instructions;
  v8::Local<v8::Number> num;
  std::size_t bLength = 0;

  // Options Stack
  v8::Local<v8::Array> fullGStack = info[0].As<v8::Array>();
  
  // Canvas Data
  // Nan::TypedArrayContents<uint8_t> data(info[1]);
  // size_t length = data.length();
  // uint_fast8_t * pixels = new uint_fast8_t [ length ]; 
  // for (size_t i = 0; i < length; ++i) {
  //   pixels[i] = (*data)[i];
  // }

  // Callback Data
  v8::Local<v8::Function> hollerback = info[2].As<v8::Function>();

  std::size_t runTimes = fullGStack->Length();
  // initialize el stacko...mmm..steak tacos
  Options *stack = new Options[runTimes];

  getParametersFromNode(fullGStack, stack);
  
  // Maintain a copy of the original buffer length
  if (fullGStack->IsObject()) {
    val = fullGStack->Get(0);
    instructions = val.As<v8::Object>();
    Nan::MaybeLocal<v8::Value> length =  Nan::Get(instructions, Nan::New("length").ToLocalChecked());
    if ( length.IsEmpty()) {
        Nan::SyntaxError("Object must have a length property");
      }
      num = length.ToLocalChecked().As<v8::Number>();
      bLength = num->NumberValue();
  }

  // Set the length, width, height manually
  for (std::size_t i = 0; i < fullGStack->Length(); /*runTimes;*/   ++i) {
    // std::cout << "its on run # " << i << std::endl;
    if(fullGStack->IsObject()) {
      val = fullGStack->Get(i);
      instructions = val.As<v8::Object>();
      

      Nan::MaybeLocal<v8::Value> length =  Nan::Get(instructions, Nan::New("length").ToLocalChecked());
      if ( length.IsEmpty()) {
        Nan::SyntaxError("Object must have a length property");
      }
      num = length.ToLocalChecked().As<v8::Number>();
  
      std::size_t intLength = num->NumberValue();
      std::cout << "Length is " << intLength << std::endl;

      stack[i].length = (intLength / 4);


      Nan::MaybeLocal<v8::Value> height =  Nan::Get(instructions, Nan::New("height").ToLocalChecked());
      if ( height.IsEmpty()) {
        Nan::SyntaxError("Object must have a height property");
      }
      num = height.ToLocalChecked().As<v8::Number>();
  
      std::size_t intHeight = num->NumberValue();
      std::cout << "Height is " << intHeight << std::endl;

      stack[i].height = intHeight;


      Nan::MaybeLocal<v8::Value> width =  Nan::Get(instructions, Nan::New("width").ToLocalChecked());
      if ( width.IsEmpty()) {
        Nan::SyntaxError("Object must have a width property");
      }
      num = width.ToLocalChecked().As<v8::Number>();
  
      std::size_t intWidth = num->NumberValue();
      std::cout << "Width is " << intWidth << std::endl;

      stack[i].width = intWidth;

    }
  }
 
  // Canvas Buffer
  v8::Local<v8::Object> bufferhandle  = info[1].As<v8::Object>();
  char* pbuf = node::Buffer::Data(bufferhandle);
  uint_fast8_t* pixels = new uint_fast8_t[ stack[0].length ];
  for(size_t i = 0; i < stack[0].length; ++i) {
    pixels[i] = pbuf[i];
  }

  Nan::Callback* holla = new Nan::Callback(hollerback);
  std::cout << "=============================" << std::endl;
  std::cout << "first position in pixels: " << (int)pixels[1] << std::endl;
  std::cout << "Original buffer length  : " << bLength << std::endl;
  std::cout << "=============================" << std::endl;
  MyBufferWorker *worker = new MyBufferWorker(holla, pixels, stack, runTimes, bLength);
  Nan::AsyncQueueWorker(worker);  
  

}





/*

NAN_METHOD(UsePngBuffer) {
  Nan::HandleScope scope;
  v8::Local<v8::Integer> integer;
  // needed
  v8::Local<v8::Value> val;
  v8::Local<v8::Object> instructions;
  v8::Local<v8::Number> num;

  // Options Stack
  v8::Local<v8::Array> fullGStack = info[0].As<v8::Array>();
  

  // Canvas Data
  // Nan::TypedArrayContents<uint8_t> data(info[1]);
  // size_t length = data.length();
  // uint_fast8_t * pixels = new uint_fast8_t [ length ]; 
  // for (size_t i = 0; i < length; ++i) {
  //   pixels[i] = (*data)[i];
  // }

  //Node buffer inc
  v8::Local<v8::Object> bufferhandle  = info[0].As<v8::Object>();
  

  // Callback Data
  v8::Local<v8::Function> hollerback = info[2].As<v8::Function>();


  std::size_t runTimes = fullGStack->Length();
  // initialize el stacko...mmm..steak tacos
  Options *stack = new Options[runTimes];

  getParametersFromNode(fullGStack, stack);
  // Set the length, width, height manually
  for (std::size_t i = 0; i < fullGStack->Length(); ++i) {
    // std::cout << "its on run # " << i << std::endl;
    if(fullGStack->IsObject()) {
      val = fullGStack->Get(i);
      instructions = val.As<v8::Object>();
      

      Nan::MaybeLocal<v8::Value> length =  Nan::Get(instructions, Nan::New("length").ToLocalChecked());
      if ( length.IsEmpty()) {
        Nan::SyntaxError("Object must have a length property");
      }
      num = length.ToLocalChecked().As<v8::Number>();
  
      std::size_t intLength = num->NumberValue();
      std::cout << "Length is " << intLength << std::endl;

      stack[i].length = intLength;


      Nan::MaybeLocal<v8::Value> height =  Nan::Get(instructions, Nan::New("height").ToLocalChecked());
      if ( height.IsEmpty()) {
        Nan::SyntaxError("Object must have a height property");
      }
      num = height.ToLocalChecked().As<v8::Number>();
  
      std::size_t intHeight = num->NumberValue();
      std::cout << "Height is " << intHeight << std::endl;

      stack[i].height = intHeight;


      Nan::MaybeLocal<v8::Value> width =  Nan::Get(instructions, Nan::New("width").ToLocalChecked());
      if ( width.IsEmpty()) {
        Nan::SyntaxError("Object must have a width property");
      }
      num = width.ToLocalChecked().As<v8::Number>();
  
      std::size_t intWidth = num->NumberValue();
      std::cout << "Width is " << intWidth << std::endl;

      stack[i].width = intWidth;

    }
  }
  
  cv::Mat imgbuf = cv::Mat(stack[0].height, stack[0].width, CV_8U, imageBuf);
  cv::Mat imgMat = cv::imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);

  Nan::Callback* holla = new Nan::Callback(hollerback);
  std::cout << "first position in pixels: " << pixels[1] << std::endl;
  MyBufferWorker *worker = new MyBufferWorker(holla, pixels, stack, runTimes);
  Nan::AsyncQueueWorker(worker);  
  

}
*/








void setOptions(const cv::Mat & m, Options *opts) 
{
  opts->height  = m.rows;
  opts->width   = m.cols;
  opts->length  = m.total();
  // cout << "Setting options -----------" << endl;
  // cout << "rows is "  <<  m.rows  << endl;
  // cout << "cols is "  <<  m.cols  << endl;
  // cout << "size is "  <<  m.size     << endl;
  // cout << "total is " <<  m.total()  << endl;
  // cout << "step is "  <<  m.step  << endl;
  // cout << "Setting options done -----" << endl;
  // cout << "roi is "   <<  m.roi   << endl;
  // cout << "type is "  <<  m.type  << endl;
  // cout << "ndims is " <<  m.ndims << endl;
  // cout << "steps is " <<  m.steps << endl;
  // cout << "vec is "   <<  m.vec   << endl;

}
// ChannelRestore
int channelSelector32LSB(uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts)
{

  uint_fast8_t r=0, g=0, b=0;
  uint_fast32_t color, oldColor, newColor;
  uint_least8_t channel = opts->channel;
  if ( (channel & 1) == 1) r = 1;
  if ( (channel & 2) == 1) g = 1;
  if ( (channel & 4) == 1) b = 1;
  
  // If none are selected, skip entirely
  // If you select r g b copy original buffer and skip

  // std::cout << "r" << (uint_fast32_t)r << endl;
  // std::cout << "g" << (uint_fast32_t)g << endl;
  // std::cout << "b" << (uint_fast32_t)b << endl;

  if (r == 0 && g == 0 && b == 0) return 0;

  size_t length = opts->length;
  for (size_t i = 0; i < length; i++) {
    color = chunk[i];
    oldColor = dupBuffer[i];
    newColor = 0;

    newColor  = (r==1) ?  color        :  oldColor;
    newColor |= (g==1) ? (color <<  8) : (oldColor <<   8);
    newColor |= (b==1) ? (color << 16) : (oldColor <<  16);
    newColor |= (255 << 24);

    dupBuffer[i] = newColor;
  
  }
  // std::cout << "Finished!" << std::endl;
  if (r && g && b) return 1;

  return 0;

}



void A2BcopyBuffer(uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts) 
{
  size_t length = opts->length, i;
  std::cout << "Length matches " << (bool)(chunk[length-1] == dupBuffer[length-1]) << std::endl;
  for ( i = 0; i < length; ++i) {
    dupBuffer[i] = chunk[i];
  }
}

void A8Bit2BcopyBuffer(uint_fast8_t *chunk, uint_fast32_t *dupBuffer, Options *opts) 
{
  size_t length = round(opts->length / 4), n = 0, i;
  uint_fast32_t color;
  if (isLSB()) {  
    for (i = 0; i < length; ++i) {
      color = (chunk[n+3] << 24) | ( chunk[n+2] << 16) | ( chunk[n+1] << 8) | chunk[n];
      dupBuffer[i] = color;
      n+=4;
    }
  } else {
    for (i = 0; i < length; ++i) {
      color = ( chunk[n] << 24) | ( chunk[n+1] << 16) | ( chunk[n+2] << 8) | chunk[n+3];
      dupBuffer[i] = color;
      n+=4;
    }
  }

}



// This is an incomplete function
void A2B8BitcopyBuffer(uint_fast32_t *chunk, uint_fast8_t *dupBuffer, Options *opts) 
{
  size_t length = opts->length, maxlength = length*4, n = 0, i;
  uint8_t red, green, blue, alpha;
  uint_fast32_t u32c;
  if (isLSB()) {  
    for (i = 0; i < length; ++i) {
      u32c = chunk[i];
      red   = (u32c & 0xFF);
      green = (u32c >>  8) &0xFF;
      blue  = (u32c >> 16) &0xFF;
      alpha = (u32c >> 24) &0xFF;
      
      dupBuffer[n+0] = red;
      dupBuffer[n+1] = green;
      dupBuffer[n+2] = blue;
      dupBuffer[n+3] = alpha;
      n+=4;
      if (n > maxlength) return;
    }
  } else {
    for (i = 0; i < length; ++i) {
      u32c = chunk[i];
      red   = (u32c >> 24)& 0xFF; 
      green = (u32c >> 16) &0xFF;
      blue  = (u32c >>  8) &0xFF;
      alpha = (u32c & 0xFF);

      dupBuffer[n+0] = red;
      dupBuffer[n+1] = green;
      dupBuffer[n+2] = blue;
      dupBuffer[n+3] = alpha;
      n+=4;
      if (n > maxlength) return;
    }
  }
}

// void B2AcopyBuffer(uint_fast32_t *chunk, uint_fast32_t *dupBuffer, size_t chunkSize) 
// {
//   for (size_t i = 0; i < chunkSize; ++i) {
//     chunk[i] = dupBuffer[i];
//   }
// }

void convertCVDataTo32bit(uint_least8_t *cvPixels, uint_fast32_t* rgbPixels, size_t size)
{
  std::cout << "converting cv data to 32 bit" << endl;
  // std::cout << "Test sample -begin-"<< endl;
  // std::cout << "RGB pixels[200]"    << rgbPixels[200] << endl;
  // std::cout << "cvPixels[600]"      << (uint_least32_t)cvPixels[600]  << endl;
  // std::cout << "cvPixels[601]"      << (uint_least32_t)cvPixels[601]  << endl;
  // std::cout << "cvPixels[602]"      << (uint_least32_t)cvPixels[602]  << endl;
  // std::cout << "Test sample -end-"  << endl;
  size_t n = 0;
  if (isLSB()) {
    // not safe!
    for (size_t i = 0; i < size; i+=3) {
      // rgbPixels[n] =  255 << 24;            // A
      // rgbPixels[n] |= cvPixels[i    ] << 16;// R
      // rgbPixels[n] |= cvPixels[i + 1] <<  8;// G
      // rgbPixels[n] |= cvPixels[i + 2];      // B
      rgbPixels[n] = (255 << 24) | (cvPixels[i] << 16) | (cvPixels[i + 1] <<  8) | (cvPixels[i + 2]);
      // msb
      // rgbPixels[n] = (cvPixels[i + 2] << 24) | (cvPixels[i + 1]  << 16) | (cvPixels[i] <<  8) | 255;
      ++n;
    }
  } else {
    std::cout << "Not supported...yet" << endl;
  }

  // std::cout << "Test sample -begin-"<< endl;
  // std::cout << "RGB pixels[200]"    << rgbPixels[200] << endl;
  // std::cout << "cvPixels[600]"      << (uint_least32_t)cvPixels[600]  << endl;
  // std::cout << "cvPixels[601]"      << (uint_least32_t)cvPixels[601]  << endl;
  // std::cout << "cvPixels[602]"      << (uint_least32_t)cvPixels[602]  << endl;
  // std::cout << "Test sample -end-"  << endl;
}





void convert32bitToCVData(uint_least8_t *cvPixels, uint_least32_t* rgbPixels , size_t size, size_t length)
{
  std::cout << "converting from 32bit to cv data" << endl;
  size_t n = 0;
  uint_fast32_t color;
  // if (isLSB()) {
  // for (size_t i = 0; i < size; ++i) {
  //   color = rgbPixels[i];
  //   cvPixels[n    ] = (color &0xFF);       // B;
  //   cvPixels[n + 1] = (color >>  8) &0xFF; // R;
  //   cvPixels[n + 2] = (color >> 16) &0xFF; // G;
  //   n +=3;
  //   if (n > size) break; 
  //   }
  // }
    if (isLSB()) {
  for (size_t i = 0; i < size; ++i) {
    color = rgbPixels[i];
    cvPixels[n + 2] = (color &0xFF);       // R;
    cvPixels[n + 1] = (color >>  8) &0xFF; // G;
    cvPixels[n + 0] = (color >> 16) &0xFF; // B;
    n += 3;
    if (n > length) break; 
    }
  }
  std::cout << "size" <<  size << endl;
  std::cout << "n" <<  n << endl;


}


void pixelate (uint_fast32_t *chunk, uint_fast32_t *dupBuffer, Options *opts) 
{
  size_t h = opts->height, w = opts->width, length = opts->length;
  uint_least8_t xspace = (std::uint_least32_t)opts->xspace;
  uint_least8_t yspace = (std::uint_least32_t)opts->yspace;
  size_t x, y, dx, dy;
  if (xspace == 0) xspace = 1; 
  if (yspace == 0) yspace = 1; 
  for (x = 0; x < h; x+=xspace) {
    for (y = 0; y < w; y+=yspace) {
      size_t index = x * w +y;
      uint_fast32_t col = chunk[index];
      // draw
      if (index + (xspace * w + yspace) > length) continue;
      for (dx = 0; dx < xspace; dx++) {
        for (dy = 0; dy < yspace; dy++) {
        size_t step =  index + (dx * w + dy);
          dupBuffer[step] = col;
        }
      }
    }
  }  
}


// void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0, comparer, u32c;
//         uint_fast8_t r, g, b, xr, xg, xb;
//         for (size_t dx = 0; dx < side; ++dx) {
//           for(size_t dy = 0; dy < side; ++dy) {
//             sIndex = (x + dx) * w + (y + dy);
//             // if (sIndex >= length) sIndex = index;
//             u32c    = pixels[ sIndex ];
//             comparer = clone[ sIndex ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   r - xr;
//             avgBlue +=  b - xb;
//             avgGreen += g - xg;
            
//           }
//         }
//         u32c = pixels[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         pixels[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
//       }
//     }
//   }

// }



// // works like sample_poisson_add
// void sample_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0, comparer, u32c;
//         uint_fast8_t r, g, b, xr, xg, xb;
//         for (size_t dx = 0; dx < side; ++dx) {
//           for(size_t dy = 0; dy < side; ++dy) {
//             sIndex = (x + dx) * w + (y + dy);
//             // if (sIndex >= length) sIndex = index;
//             u32c    = pixels[ sIndex ];
//             comparer = clone[ sIndex ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   r - xr;
//             avgBlue +=  b - xb;
//             avgGreen += g - xg;
            
//           }
//         }
//         u32c = clone[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         pixels[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
//       }
//     }
//   }

// }




void poisson_blend_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  
  int_least32_t avgR = 0, avgG = 0, avgB = 0;
  // double cr, cg, cb;
  // uint_least32_t cr, cg, cb; // might need to change this
  uint_least32_t u32c, u32cP;
  // auto cr = 0, cg = 0, cb = 0;
  // size_t r,g,b,xr,xg,xb;
  size_t h = opts->height, w = opts->width, index, sIndex, x, x1, y, y1, h1, w1, dx, dy;
  for (x = 0, x1 = 1, h1 = h-1; x1 < h1; ++x, ++x1 ) {
    for (y = 0, y1 = 1, w1 = w-1; y1 < w1; ++y, ++y1 ) {
      index = x1 * w + y1;
      avgR = avgG = avgB = 0;
      for (dx = 0; dx < 3; dx++) {
        for (dy = 0; dy < 3; ++dy) {
  
          sIndex = (x + dx) * w + (y + dy);
  
          u32c = pixels[sIndex];
          u32cP = clone[sIndex];
          avgR += (u32c &0xFF)          - (u32cP &0xFF);
          avgG += ((u32c >>  8) &0xFF ) - ((u32cP >>  8)&0xFF );
          avgB += ((u32c >> 16) &0xFF ) - ((u32cP >> 16)&0xFF );

 
        }
      }
  
      u32c = pixels[index];

      u32c = (255 << 24) | ((uint_fast8_t)(((u32c >> 16)&0xFF) + (avgB / 9))) << 16 | ((uint_fast8_t)(((u32c >>  8)&0xFF) + (avgG / 9))) <<  8 | ((uint_fast8_t)((u32c &0xFF) + (avgR / 9)));
      std::cout << "norm: " << u32c << std::endl;

      u32c = (255 << 24) | (((u32c >> 16)&0xFF) + (avgB / 9)) << 16 | (((u32c >>  8)&0xFF) + (avgG / 9)) <<  8 | ((u32c &0xFF) + (avgR / 9));
      std::cout << "no_conversion: " << u32c << std::endl;
      clone[index] = u32c;
      // clone[index] = (255 << 24) | 
      //   ((uint_fast8_t)(((u32c >> 16)&0xFF) + (avgB / 9))) << 16 | 
      //   ((uint_fast8_t)(((u32c >>  8)&0xFF) + (avgG / 9))) <<  8 |
      //   ((uint_fast8_t)((u32c &0xFF) + (avgR / 9)));



    }
  }

}



void poisson_blend_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;

  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   dupBuffer[ m[k]    ] - chunk[ m[k]     ];
            avgBlue +=  dupBuffer[ m[k] + 1] - chunk[ m[k] + 1 ];
            avgGreen += dupBuffer[ m[k] + 2] - chunk[ m[k] + 2 ];
        }

     

        dupBuffer[index  ] =  chunk[index  ] + (avgRed   / ml);
        dupBuffer[index+1] =  chunk[index+1] + (avgGreen / ml);
        dupBuffer[index+2] =  chunk[index+2] + (avgBlue  / ml);

    }
  }
}






void poisson_blend_add_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;

  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   dupBuffer[ m[k]    ] - chunk[ m[k]     ];
            avgBlue +=  dupBuffer[ m[k] + 1] - chunk[ m[k] + 1 ];
            avgGreen += dupBuffer[ m[k] + 2] - chunk[ m[k] + 2 ];
        }
        avgRed   = chunk[index   ] + (avgRed/ ml);
        avgGreen = chunk[index +1] + (avgGreen/ ml);
        avgBlue  = chunk[index +2] + (avgBlue/ ml);


     
        // This should works
        // dupBuffer[index  ] =  (avgRed < 0)   ? 255 - ((~-avgRed) % 256)   : avgRed % 256;
        // dupBuffer[index+1] =  (avgGreen < 0) ? 255 - ((~-avgGreen) % 256) : avgGreen % 256;
        // dupBuffer[index+2] =  (avgBlue < 0)  ? 255 - ((~-avgBlue) % 256)  : avgBlue % 256;
        dupBuffer[index   ] = avgRed > 255 ? 255 : avgRed < 0 ? 0 : avgRed;
        dupBuffer[index+1 ] = avgGreen > 255 ? 255 : avgGreen < 0 ? 0 : avgGreen;
        dupBuffer[index+2 ] = avgBlue > 255 ? 255 : avgBlue < 0 ? 0 : avgBlue;
    }
  }
}




void poisson_blend_subtract_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;

  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   dupBuffer[ m[k]    ] - chunk[ m[k]     ];
            avgBlue +=  dupBuffer[ m[k] + 1] - chunk[ m[k] + 1 ];
            avgGreen += dupBuffer[ m[k] + 2] - chunk[ m[k] + 2 ];
        }
        avgRed   = chunk[index   ] - (avgRed/ ml);
        avgGreen = chunk[index +1] - (avgGreen/ ml);
        avgBlue  = chunk[index +2] - (avgBlue/ ml);

     
        // This should works
        dupBuffer[index  ] =  (avgRed < 0)   ? 255 - ((~-avgRed) % 256)   : avgRed % 256;
        dupBuffer[index+1] =  (avgGreen < 0) ? 255 - ((~-avgGreen) % 256) : avgGreen % 256;
        dupBuffer[index+2] =  (avgBlue < 0)  ? 255 - ((~-avgBlue) % 256)  : avgBlue % 256;

    }
  }
}




void poisson_blend_multiply_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;

  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   dupBuffer[ m[k]    ] - chunk[ m[k]     ];
            avgBlue +=  dupBuffer[ m[k] + 1] - chunk[ m[k] + 1 ];
            avgGreen += dupBuffer[ m[k] + 2] - chunk[ m[k] + 2 ];
        }
        avgRed   = dupBuffer[index   ] + (avgRed/ ml);
        avgGreen = dupBuffer[index +1] + (avgGreen/ ml);
        avgBlue  = dupBuffer[index +2] + (avgBlue/ ml);

     
        // This should works
        dupBuffer[index  ] =  (avgRed < 0)   ? 255 - ((~-avgRed) % 256)   : avgRed % 256;
        dupBuffer[index+1] =  (avgGreen < 0) ? 255 - ((~-avgGreen) % 256) : avgGreen % 256;
        dupBuffer[index+2] =  (avgBlue < 0)  ? 255 - ((~-avgBlue) % 256)  : avgBlue % 256;

    }
  }
}



void poisson_blend_divide_better_math (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;

  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   dupBuffer[ m[k]    ] - chunk[ m[k]     ];
            avgBlue +=  dupBuffer[ m[k] + 1] - chunk[ m[k] + 1 ];
            avgGreen += dupBuffer[ m[k] + 2] - chunk[ m[k] + 2 ];
        }
        avgRed   = dupBuffer[index   ] - (avgRed/ ml);
        avgGreen = dupBuffer[index +1] - (avgGreen/ ml);
        avgBlue  = dupBuffer[index +2] - (avgBlue/ ml);

     
        // This should works
        dupBuffer[index  ] =  (avgRed < 0)   ? 255 - ((~-avgRed) % 256)   : avgRed % 256;
        dupBuffer[index+1] =  (avgGreen < 0) ? 255 - ((~-avgGreen) % 256) : avgGreen % 256;
        dupBuffer[index+2] =  (avgBlue < 0)  ? 255 - ((~-avgBlue) % 256)  : avgBlue % 256;

    }
  }
}


void poisson_blend_add_source_void (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
 
  double cr, cg, cb;
  uint_least32_t u32c, u32cP;
  int_least32_t avgR = 0, avgG = 0, avgB = 0;
  size_t h = opts->height, w = opts->width, index, sIndex, x, x1, y, y1, h1, w1, dx, dy;
  for (x = 0, x1 = 1, h1 = h-1; x1 < h1; ++x, ++x1 ) {
    for (y = 0, y1 = 1, w1 = w-1; y1 < w1; ++y, ++y1 ) {
      index = x1 * w + y1;
      avgR = avgG = avgB = 0;
      for (dx = 0; dx < 3; dx++) {
        for (dy = 0; dy < 3; ++dy) {
  
          sIndex = (x + dx) * w + (y + dy);

          u32c = pixels[sIndex];
          u32cP = clone[sIndex];
          avgR += (u32c &0xFF)          - (u32cP &0xFF);
          avgG += ((u32c >>  8) &0xFF ) - ((u32cP >>  8)&0xFF );
          avgB += ((u32c >> 16) &0xFF ) - ((u32cP >> 16)&0xFF );

        }
      }
 
      // Ready to blend
      u32c = pixels[index];

      cr = (u32c &0xFF)        + (avgR / 9);
      cg = ((u32c >>  8)&0xFF) + (avgG / 9);
      cb = ((u32c >> 16)&0xFF) + (avgB / 9);

      avgR = min(max((int)cr,0), 255);
      avgG = min(max((int)cg,0), 255);
      avgB = min(max((int)cb,0), 255);


      clone[index] = (255 << 24) | (  avgB << 16) | ( avgG << 8) | avgR ;


    }
  }

}


void poisson_blend_subtract_source_void (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  
  double cr, cg, cb;
  uint_least32_t u32c, u32cP;
  int_least32_t avgR = 0, avgG = 0, avgB = 0;
  size_t h = opts->height, w = opts->width, index, sIndex, x, x1, y, y1, h1, w1, dx, dy;
  for (x = 0, x1 = 1, h1 = h-1; x1 < h1; ++x, ++x1 ) {
    for (y = 0, y1 = 1, w1 = w-1; y1 < w1; ++y, ++y1 ) {
      index = x1 * w + y1;
      avgR = avgG = avgB = 0;
      for (dx = 0; dx < 3; dx++) {
        for (dy = 0; dy < 3; ++dy) {
  
          sIndex = (x + dx) * w + (y + dy);
     
          u32c = pixels[sIndex];
          u32cP = clone[sIndex];
          avgR += (u32c &0xFF)          - (u32cP &0xFF);
          avgG += ((u32c >>  8) &0xFF ) - ((u32cP >>  8)&0xFF );
          avgB += ((u32c >> 16) &0xFF ) - ((u32cP >> 16)&0xFF );

        }
      }

      // Ready to blend
      u32c = pixels[index];

      cr = (u32c &0xFF)        - (avgR / 9);
      cg = ((u32c >>  8)&0xFF) - (avgG / 9);
      cb = ((u32c >> 16)&0xFF) - (avgB / 9);

      avgR = min(max((int)cr,0), 255);
      avgG = min(max((int)cg,0), 255);
      avgB = min(max((int)cb,0), 255);


      clone[index] = (255 << 24) | (  avgB << 16) | ( avgG << 8) | avgR ;


    }
  }

}



void poisson_blend_add_sample_void (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  double cr, cg, cb;
  uint_least32_t u32c, u32cP;
  int_least32_t avgR = 0, avgG = 0, avgB = 0;
  size_t h = opts->height, w = opts->width, index, sIndex, x, x1, y, y1, h1, w1, dx, dy;
  for (x = 0, x1 = 1, h1 = h-1; x1 < h1; ++x, ++x1 ) {
    for (y = 0, y1 = 1, w1 = w-1; y1 < w1; ++y, ++y1 ) {
      index = x1 * w + y1;
      avgR = avgG = avgB = 0;
      for (dx = 0; dx < 3; dx++) {
        for (dy = 0; dy < 3; ++dy) {
  
          sIndex = (x + dx) * w + (y + dy);
          u32c = pixels[sIndex];
          u32cP = clone[sIndex];
          avgR += (u32c &0xFF)          - (u32cP &0xFF);
          avgG += ((u32c >>  8) &0xFF ) - ((u32cP >>  8)&0xFF );
          avgB += ((u32c >> 16) &0xFF ) - ((u32cP >> 16)&0xFF );

        }
      }
 
      u32c = clone[index];

      cr = (u32c &0xFF)        + (avgR / 9);
      cg = ((u32c >>  8)&0xFF) + (avgG / 9);
      cb = ((u32c >> 16)&0xFF) + (avgB / 9);

      avgR = min(max((int)cr,0), 255);
      avgG = min(max((int)cg,0), 255);
      avgB = min(max((int)cb,0), 255);


      clone[index] = (255 << 24) | (  avgB << 16) | ( avgG << 8) | avgR ;


    }
  }

}


void poisson_blend_subtract_sample_void (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  double cr, cg, cb;
  uint_least32_t u32c, u32cP;
  int_least32_t avgR = 0, avgG = 0, avgB = 0;
  size_t h = opts->height, w = opts->width, index, sIndex, x, x1, y, y1, h1, w1, dx, dy;
  for (x = 0, x1 = 1, h1 = h-1; x1 < h1; ++x, ++x1 ) {
    for (y = 0, y1 = 1, w1 = w-1; y1 < w1; ++y, ++y1 ) {
      index = x1 * w + y1;
      avgR = avgG = avgB = 0;
      for (dx = 0; dx < 3; dx++) {
        for (dy = 0; dy < 3; ++dy) {
  
          sIndex = (x + dx) * w + (y + dy);
          u32c = pixels[sIndex];
          u32cP = clone[sIndex];
          avgR += (u32c &0xFF)          - (u32cP &0xFF);
          avgG += ((u32c >>  8) &0xFF ) - ((u32cP >>  8)&0xFF );
          avgB += ((u32c >> 16) &0xFF ) - ((u32cP >> 16)&0xFF );

        }
      }
 
      u32c = clone[index];

      cr = (u32c &0xFF)        - (avgR / 9);
      cg = ((u32c >>  8)&0xFF) - (avgG / 9);
      cb = ((u32c >> 16)&0xFF) - (avgB / 9);

      avgR = min(max((int)cr,0), 255);
      avgG = min(max((int)cg,0), 255);
      avgB = min(max((int)cb,0), 255);


      clone[index] = (255 << 24) | (  avgB << 16) | ( avgG << 8) | avgR ;


    }
  }

}






// void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   std::vector<size_t> m (9);
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height - 1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width - 1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t comparer, u32c;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0;
//         uint_fast8_t r, g, b, xr, xg, xb;
//           m[0] = index-((w) + 1);
//           m[1] = index-(w);
//           m[2] = index-(w) + 1;
//           m[3] = index-1;
//           m[4] = index;
//           m[5] = index+1;
//           m[6] = index+((w)-1);
//           m[7] = index+ (w);
//           m[8] = index+(w) + 1;

//           for (size_t k = 0; k < ml; ++k) {
//             u32c    = pixels[ m[k] ];
//             comparer = clone[ m[k] ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   min((uint_least32_t)r + xr, (uint_least32_t)0x00);
//             avgBlue +=  min((uint_least32_t)b + xb, (uint_least32_t)0x00);
//             avgGreen += min((uint_least32_t)g + xg, (uint_least32_t)0x00);
//           }
//         // for (size_t dx = 0; dx < side; ++dx) {
//         //   for(size_t dy = 0; dy < side; ++dy) {
//         //     sIndex = (x + dy) * w + (y + dx);
//         //     // if (sIndex >= length) sIndex = index;
//         //     u32c    = pixels[ sIndex ];
//         //     comparer = clone[ sIndex ];
//         //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//         //     avgRed +=   (r - xr);
//         //     avgBlue +=  (b - xb);
//         //     avgGreen += (g - xg);
            
//         //   }
//         // }
//         u32c = pixels[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
//         avgBlue  = 0;
//         avgRed   = 0;
//         avgGreen = 0;
//       }
//     }
//   }
// }
// void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   std::vector<size_t> m (9);
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height - 1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width - 1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t comparer, u32c;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0;
//         uint_fast8_t r, g, b, xr, xg, xb;
//           m[0] = index-((w) + 1);
//           m[1] = index-(w);
//           m[2] = index-(w) + 1;
//           m[3] = index-1;
//           m[4] = index;
//           m[5] = index+1;
//           m[6] = index+((w)-1);
//           m[7] = index+ (w);
//           m[8] = index+(w) + 1;

//           for (size_t k = 0; k < ml; ++k) {
//             u32c    = pixels[ m[k] ];
//             comparer = clone[ m[k] ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   min((uint_least32_t)r + xr, (uint_least32_t)0x00);
//             avgBlue +=  min((uint_least32_t)b + xb, (uint_least32_t)0x00);
//             avgGreen += min((uint_least32_t)g + xg, (uint_least32_t)0x00);
//           }
//         // for (size_t dx = 0; dx < side; ++dx) {
//         //   for(size_t dy = 0; dy < side; ++dy) {
//         //     sIndex = (x + dy) * w + (y + dx);
//         //     // if (sIndex >= length) sIndex = index;
//         //     u32c    = pixels[ sIndex ];
//         //     comparer = clone[ sIndex ];
//         //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//         //     avgRed +=   (r - xr);
//         //     avgBlue +=  (b - xb);
//         //     avgGreen += (g - xg);
            
//         //   }
//         // }
//         u32c = pixels[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
//         avgBlue  = 0;
//         avgRed   = 0;
//         avgGreen = 0;
//       }
//     }
//   }
// }


// void source_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   std::vector<size_t> m (9);
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0, comparer, u32c;
//         uint_fast8_t r, g, b, xr, xg, xb;
//           m[0] = index-((w) + 1);
//           m[1] = index-(w);
//           m[2] = index-(w) + 1;
//           m[3] = index-1;
//           m[4] = index;
//           m[5] = index+1;
//           m[6] = index+((w)-1);
//           m[7] = index+ (w);
//           m[8] = index+(w) + 1;

//           for (size_t k = 0; k < ml; ++k) {
//             u32c    = pixels[ m[k] ];
//             comparer = clone[ m[k] ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   max((uint_least8_t)r - xr, 0);
//             avgBlue +=  max((uint_least8_t)b - xb, 0);
//             avgGreen += max((uint_least8_t)g - xg, 0);
//           }
//         // for (size_t dx = 0; dx < side; ++dx) {
//         //   for(size_t dy = 0; dy < side; ++dy) {
//         //     sIndex = (x + dx) * w + (y + dy);
//         //     // if (sIndex >= length) sIndex = index;
//         //     u32c    = pixels[ sIndex ];
//         //     comparer = clone[ sIndex ];
//         //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//         //     avgRed +=   r - xr;
//         //     avgBlue +=  b - xb;
//         //     avgGreen += g - xg;
            
//         //   }
//         // }
//         u32c = pixels[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         clone[index] =  LSBUINT32COLOR( (r + (avgRed / ml)), (g + (avgGreen / ml)), ( b + (avgBlue / ml)) );
//       }
//     }
//   }

// }


// void source_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0, comparer, u32c;
//         uint_fast8_t r, g, b, xr, xg, xb;
//         for (size_t dx = 0; dx < side; ++dx) {
//           for(size_t dy = 0; dy < side; ++dy) {
//             sIndex = (x + dx) * w + (y + dy);
//             // if (sIndex >= length) sIndex = index;
//             u32c    = pixels[ sIndex ];
//             comparer = clone[ sIndex ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   r - xr;
//             avgBlue +=  b - xb;
//             avgGreen += g - xg;
            
//           }
//         }
//         u32c = pixels[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r - (avgRed / ml)), (uint_fast8_t)(g - (avgGreen / ml)), (uint_fast8_t)( b - (avgBlue / ml)) );
//       }
//     }
//   }
// }




void source_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // size_t length = opts->length;
  std::vector<size_t> m (9);
  if (isLSB()) { 
    for (size_t x=0,x1=1, h1 = opts->height - 1; x1 < h1; ++x, ++x1) {
      for (size_t y=0,y1=1,w = opts->width, w1 = opts->width - 1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1); 
        // movement kernel
        size_t ml = 9; //, side = 3, sIndex;
        uint_fast32_t comparer, u32c;
        uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0;
        uint_fast8_t r, g, b, xr, xg, xb;
          m[0] = index-((w) + 1);
          m[1] = index-(w);
          m[2] = index-(w) + 1;
          m[3] = index-1;
          m[4] = index;
          m[5] = index+1;
          m[6] = index+((w)-1);
          m[7] = index+ (w);
          m[8] = index+(w) + 1;

          for (size_t k = 0; k < ml; ++k) {
            u32c    = pixels[ m[k] ];
            comparer = clone[ m[k] ];
            UNPACKLSBUINT32COLOR(r,g,b, u32c)
            UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
            avgRed +=   max((uint_least8_t)r - xr, 0);
            avgBlue +=  max((uint_least8_t)b - xb, 0);
            avgGreen += max((uint_least8_t)g - xg, 0);
          }
        // for (size_t dx = 0; dx < side; ++dx) {
        //   for(size_t dy = 0; dy < side; ++dy) {
        //     sIndex = (x + dy) * w + (y + dx);
        //     // if (sIndex >= length) sIndex = index;
        //     u32c    = pixels[ sIndex ];
        //     comparer = clone[ sIndex ];
        //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
        //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
        //     avgRed +=   (r - xr);
        //     avgBlue +=  (b - xb);
        //     avgGreen += (g - xg);
            
        //   }
        // }
        u32c = pixels[index];
        UNPACKLSBUINT32COLOR(r,g,b, u32c)
        clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
        avgBlue  = 0;
        avgRed   = 0;
        avgGreen = 0;
      }
    }
  }
}




void sample_poisson_add (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // size_t length = opts->length;
  std::vector<size_t> m (9);
  if (isLSB()) { 
    for (size_t x=0,x1=1, h1 = opts->height - 1; x1 < h1; ++x, ++x1) {
      for (size_t y=0,y1=1,w = opts->width, w1 = opts->width - 1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1); 
        // movement kernel
        size_t ml = 9; //, side = 3, sIndex;
        uint_fast32_t comparer, u32c;
        uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0;
        uint_fast8_t r, g, b, xr, xg, xb;
          m[0] = index-((w) + 1);
          m[1] = index-(w);
          m[2] = index-(w) + 1;
          m[3] = index-1;
          m[4] = index;
          m[5] = index+1;
          m[6] = index+((w)-1);
          m[7] = index+ (w);
          m[8] = index+(w) + 1;

          for (size_t k = 0; k < ml; ++k) {
            u32c    = pixels[ m[k] ];
            comparer = clone[ m[k] ];
            UNPACKLSBUINT32COLOR(r,g,b, u32c)
            UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
            avgRed +=   max((uint_least8_t)r + xr, 0);
            avgBlue +=  max((uint_least8_t)b + xb, 0);
            avgGreen += max((uint_least8_t)g + xg, 0);
          }
        // for (size_t dx = 0; dx < side; ++dx) {
        //   for(size_t dy = 0; dy < side; ++dy) {
        //     sIndex = (x + dy) * w + (y + dx);
        //     // if (sIndex >= length) sIndex = index;
        //     u32c    = pixels[ sIndex ];
        //     comparer = clone[ sIndex ];
        //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
        //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
        //     avgRed +=   (r - xr);
        //     avgBlue +=  (b - xb);
        //     avgGreen += (g - xg);
            
        //   }
        // }
        u32c = clone[index];
        UNPACKLSBUINT32COLOR(r,g,b, u32c)
        clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
        avgBlue  = 0;
        avgRed   = 0;
        avgGreen = 0;
      }
    }
  }
}





void sample_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // size_t length = opts->length;
  std::vector<size_t> m (9);
  if (isLSB()) { 
    for (size_t x=0,x1=1, h1 = opts->height - 1; x1 < h1; ++x, ++x1) {
      for (size_t y=0,y1=1,w = opts->width, w1 = opts->width - 1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1); 
        // movement kernel
        size_t ml = 9; //, side = 3, sIndex;
        uint_fast32_t comparer, u32c;
        uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0;
        uint_fast8_t r, g, b, xr, xg, xb;
          m[0] = index-((w) + 1);
          m[1] = index-(w);
          m[2] = index-(w) + 1;
          m[3] = index-1;
          m[4] = index;
          m[5] = index+1;
          m[6] = index+((w)-1);
          m[7] = index+ (w);
          m[8] = index+(w) + 1;

          for (size_t k = 0; k < ml; ++k) {
            u32c    = pixels[ m[k] ];
            comparer = clone[ m[k] ];
            UNPACKLSBUINT32COLOR(r,g,b, u32c)
            UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
            avgRed +=   max((uint_least8_t)r - xr, 0);
            avgBlue +=  max((uint_least8_t)b - xb, 0);
            avgGreen += max((uint_least8_t)g - xg, 0);
          }
        // for (size_t dx = 0; dx < side; ++dx) {
        //   for(size_t dy = 0; dy < side; ++dy) {
        //     sIndex = (x + dy) * w + (y + dx);
        //     // if (sIndex >= length) sIndex = index;
        //     u32c    = pixels[ sIndex ];
        //     comparer = clone[ sIndex ];
        //     UNPACKLSBUINT32COLOR(r,g,b, u32c)
        //     UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
        //     avgRed +=   (r - xr);
        //     avgBlue +=  (b - xb);
        //     avgGreen += (g - xg);
            
        //   }
        // }
        u32c = clone[index];
        UNPACKLSBUINT32COLOR(r,g,b, u32c)
        clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r + (avgRed / ml)), (uint_fast8_t)(g + (avgGreen / ml)), (uint_fast8_t)( b + (avgBlue / ml)) );
        avgBlue  = 0;
        avgRed   = 0;
        avgGreen = 0;
      }
    }
  }
}



// void sample_poisson_subtract (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
// {
//   size_t length = opts->length;
//   if (isLSB()) { 
//     for (size_t x=0,x1=1,h = opts->height, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
//       for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
//         // starting at 1,1
//         size_t index = (x1*w+y1); 
//         // movement kernel
//         size_t ml = 9, side = 3, sIndex;
//         uint_fast32_t avgRed = 0, avgBlue = 0, avgGreen = 0, comparer, u32c;
//         uint_fast8_t r, g, b, xr, xg, xb;
//         for (size_t dx = 0; dx < side; ++dx) {
//           for(size_t dy = 0; dy < side; ++dy) {
//             sIndex = (x + dx) * w + (y + dy);
//             // if (sIndex >= length) sIndex = index;
//             u32c    = pixels[ sIndex ];
//             comparer = clone[ sIndex ];
//             UNPACKLSBUINT32COLOR(r,g,b, u32c)
//             UNPACKLSBUINT32COLOR(xr, xg, xb, comparer)
//             avgRed +=   r - xr;
//             avgBlue +=  b - xb;
//             avgGreen += g - xg;
            
//           }
//         }
//         u32c = clone[index];
//         UNPACKLSBUINT32COLOR(r,g,b, u32c)
//         clone[index] =  LSBUINT32COLOR( (uint_fast8_t)(r - (avgRed / ml)), (uint_fast8_t)(g - (avgGreen / ml)), (uint_fast8_t)( b - (avgBlue / ml)) );
//       }
//     }
//   }
// }



// ?????
void crunch (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  size_t skip = opts->threshold, length = opts->length, i = 0;
  // size_t skippy = skip * 4;
  for (; i < length; ++i) {
    if (i&skip) {
      clone[i] = pixels[i];
    } else {
      clone[i] = clone[i];
    }
  }
}


// Something overflows :(
void deplace  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  size_t w = opts->width, h = opts->height,w1 = w-1, h1 = h - 1;;
  size_t index, target, x, y, shiftx, shifty, length = opts->length;
  uint_least8_t depth = opts->threshold;
  // uint_least8_t xspace = opts->xspace, yspace = opts->yspace;
  // uint_least8_t weight = opts->weight, stroke = opts->stroke;
  double ca = cos(50), sb = sin(120);
  double mx = 0, my = 0;
      mx += ( ca > 0) ? ca * 2 : abs(ca);
      my += ( sb > 0) ? sb * 2 : abs(sb);
  for(x = 0; x < h; ++x) {
    for(y = 0; y < w; ++y) {
      index = x * w + y;
      shiftx = (size_t)x + pixels[index] / MAX32 * -mx * depth;
      shifty = (size_t)y + pixels[index] / MAX32 * -my * depth;
      // clamp
      if(shiftx > w1){
          shiftx = w1;
      }
      if(shifty > h1){
          shifty = h1;
      }

      target = shifty * w + shiftx;
      if (target > length) target = length;
      clone[index] = pixels[target];
    }
  }
}


void zoomth (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 
  // magnify by 4x

  size_t length = opts->length - 1, i = 1, n = 0;
  uint_fast8_t r, r1, r2, g, g1, g2, b, b1, b2;
  if (isLSB()) {
      // start i at 1 to avoid checking
      // Finish 1 early to avoid checking
    clone[n] = pixels[n];
    ++n;
    while (n < length-1) {
    // LSB
      uint_fast32_t u32c = pixels[i];
      uint_fast32_t u32cN = pixels[i+1];
      uint_fast32_t u32cP = pixels[i-1];
      UNPACKLSBUINT32COLOR(r,g,b, u32c)
      UNPACKLSBUINT32COLOR(r1,g1,b1, u32cN)
      UNPACKLSBUINT32COLOR(r2,g2,b2, u32cP)
      
      if (r == 0) ++r; 
      if (g == 0) ++g; 
      if (b == 0) ++b; 

      uint_fast8_t loginatorR = (uint_fast8_t)((r + r1 + r2) / log2(r + r1 + r2));
      uint_fast8_t loginatorG = (uint_fast8_t)((g + g1 + g2) / log2(g + g1 + g2));
      uint_fast8_t loginatorB = (uint_fast8_t)((b + b1 + b2) / log2(b + b1 + b2));

      uint_fast32_t xndf = LSBUINT32COLOR(loginatorR, loginatorB, loginatorG);
      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = xndf;
      clone[n+2] = u32cP;
      clone[n+3] = u32c;
      ++i;
      n += 4;

    }
    clone[n+1] = 0xFFFFFFFF;
  } else {
    clone[n] = pixels[n];
    ++n;
    while (n < length-1) {

    // MSB
      uint_fast32_t u32c = pixels[i];
      uint_fast32_t u32cN = pixels[i+1];
      uint_fast32_t u32cP = pixels[i-1];
      
      UNPACKMSBUINT32COLOR(r,g,b, u32c)
      UNPACKMSBUINT32COLOR(r1,g1,b1, u32cN)
      UNPACKMSBUINT32COLOR(r2,g2,b2, u32cP)
    
      // var r = (u32c >> 24) &0xFF;
      // var g = (u32c >> 16) &0xff;
      // var b = (u32c >>  8) &0xff;
      
      // var r1 = (u32cN >> 24) &0xFF;
      // var g1 = (u32cN >> 16) &0xff;
      // var b1 = (u32cN >>  8) &0xff;
      
      // var r2 = (u32cP >> 24) &0xFF;
      // var g2 = (u32cP >> 16) &0xff;
      // var b2 = (u32cP >>  8) &0xff;

      if (r == 0) ++r; 
      if (g == 0) ++g; 
      if (b == 0) ++b; 

      uint_fast8_t loginatorR = (uint_fast8_t)((r + r1 + r2) / log2(r + r1 + r2));
      uint_fast8_t loginatorG = (uint_fast8_t)((g + g1 + g2) / log2(g + g1 + g2));
      uint_fast8_t loginatorB = (uint_fast8_t)((b + b1 + b2) / log2(b + b1 + b2));

      uint_fast32_t xndf = MSBUINT32COLOR(loginatorR, loginatorB, loginatorG);
      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = xndf;
      clone[n+2] = u32cP;
      clone[n+3] = u32c;
      ++i;
      n += 4;
      
    }
    clone[n+1] = 0xFFFFFFFF;
  }
}





void zoom_b (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{  
  // resize by 3x

  size_t length = opts->length-1, i = 0, n = 0;
  uint_fast8_t r, r1, g, g1, g2, b, b1;
  if (isLSB()) {
    while (n < length-1) {
    // LSB
      uint_fast32_t u32c = pixels[i];
      
      UNPACKLSBUINT32COLOR(r,g,b, u32c)
         
      r1 = std::max(r - 127, 0);
      g1 = std::max(g - (uint_fast8_t)(g/2), 0);
      g2 = std::max(g - (uint_fast8_t)(b/2), 0);
      b1 = std::min(b + (uint_fast8_t)(b/2), 255);

      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = LSBUINT32COLOR(r1, g1, b1);
      clone[n+2] = LSBUINT32COLOR(r1, g2, b1);
      ++i;
      n += 3;

    }
  } else {
    while (n < length-1) {
    // MSB
      uint_fast32_t u32c = pixels[i];
      
      UNPACKMSBUINT32COLOR(r,g,b, u32c)
         
      r1 = std::max(r - 127, 0);
      g1 = std::max(g - (uint_fast8_t)(g/2), 0);
      g2 = std::max(g - (uint_fast8_t)(b/2), 0);
      b1 = std::min(b + (uint_fast8_t)(b/2), 255);

      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = MSBUINT32COLOR(r1, g1, b1);
      clone[n+2] = MSBUINT32COLOR(r1, g2, b1);
      ++i;
      n += 3;

    }
  }

}




void zoom_r (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{  
  // resize by 3x

  size_t length = opts->length-1, i = 0, n = 0;
  uint_fast8_t r, r1, r2, g, g1, b, b1;
  if (isLSB()) {
    while (n < length-1) {
    // LSB
      uint_fast32_t u32c = pixels[i];
      
      UNPACKLSBUINT32COLOR(r,g,b, u32c)
         
      r1 = std::max(r - 127, 0);
      r2 = std::max(r - (uint_fast8_t)(b/2), 0);
      g1 = std::max(g - (uint_fast8_t)(g/2), 0);
      b1 = std::min(b + (uint_fast8_t)(b/2), 255);

      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = LSBUINT32COLOR(r1, g1, b1);
      clone[n+2] = LSBUINT32COLOR(r1, r2, b1);
      ++i;
      n += 3;

    }
  } else {
    while (n < length-1) {
    // MSB
      uint_fast32_t u32c = pixels[i];
      
      UNPACKMSBUINT32COLOR(r,g,b, u32c)
         
      r1 = std::max(r - 127, 0);
      r2 = std::max(g - (uint_fast8_t)(b/2), 0);
      g1 = std::max(g - (uint_fast8_t)(g/2), 0);
      b1 = std::min(b + (uint_fast8_t)(b/2), 255);

      // uint_fast32_t xndf = (255 << 24) | ( loginatorB << 16) | ( loginatorG << 8) | loginatorR;

      clone[n  ] = u32c;
      clone[n+1] = MSBUINT32COLOR(r1, g1, b1);
      clone[n+2] = MSBUINT32COLOR(r1, r2, b1);
      ++i;
      n += 3;

    }
  }

}



void rgb_2_bgr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // type: channelshift

  // size_t h = opts->height, w = opts->width, dl = opts->length,i; 
  size_t dl = opts->length, i; 
  uint_fast8_t r, g, b;
  if (isLSB()) {
    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKLSBUINT32COLOR(r,g,b,u32c)

      clone[i] = LSBUINT32COLOR(b,g,r);
    }

  } else {

    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKMSBUINT32COLOR(r,g,b,u32c)

      clone[i] = MSBUINT32COLOR(b,g,r);

    }

  }

}




void rgb_2_gbr (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // type: channelshift

  // size_t h = opts->height, w = opts->width, dl = opts->length,i; 
  size_t dl = opts->length,i; 
  uint_fast8_t r, g, b;
  if (isLSB()) {
    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKLSBUINT32COLOR(r,g,b,u32c)

      clone[i] = LSBUINT32COLOR(g,b,r);
    }

  } else {

    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKMSBUINT32COLOR(r,g,b,u32c)

      clone[i] = MSBUINT32COLOR(g,b,r);

    }

  }

}


void rgb_2_grb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // type: channelshift

  // size_t h = opts->height, w = opts->width, dl = opts->length,i; 
  size_t dl = opts->length,i; 
  uint_fast8_t r, g, b;
  if (isLSB()) {
    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKLSBUINT32COLOR(r,g,b,u32c)

      clone[i] = LSBUINT32COLOR(g,r,b);
    }

  } else {

    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKMSBUINT32COLOR(r,g,b,u32c)

      clone[i] = MSBUINT32COLOR(g,r,b);

    }

  }

}



void rgb_2_brg (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // type: channelshift

  // size_t h = opts->height, w = opts->width, dl = opts->length,i; 
  size_t dl = opts->length,i; 
  uint_fast8_t r, g, b;
  if (isLSB()) {
    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKLSBUINT32COLOR(r,g,b,u32c)

      clone[i] = LSBUINT32COLOR(b,r,g);
    }

  } else {

    for (i = 0; i < dl; ++i) {
      uint_fast32_t u32c = pixels[i];

      UNPACKMSBUINT32COLOR(r,g,b,u32c)

      clone[i] = MSBUINT32COLOR(b,r,g);

    }

  }

}



void pixelsort (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 

  // type: drift
  
  size_t h = opts->height, w = opts->width, index, x, y;

  std::vector<uint_fast32_t> unsorted(w);
  for (x = 0; x < h; ++x ) {
    for (y = 0; y < w; ++y ) {
      index = x * w + y;
      unsorted[y] = pixels[index];
    }
    // stable sort uses merge sort
    std::stable_sort( unsorted.begin(), unsorted.end());

    for(y = 0; y < w; ++y) {
      index = x * w + y;
      clone[index] = unsorted[y];
    }

  }
  
}


void wet_reverb (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 
  // http://stackoverflow.com/questions/5318989/reverb-algorithm
  // type: drift
  // size_t h = opts->height, w = opts->width, length = opts->length,reverbed, i;
  size_t w = opts->width, length = opts->length, reverbed, i;
  // delayMilliseconds = opts->threshold;
  double decay = opts->depth; // Level of reverb;
  double delaySamples = w * 44.1; 
  for (i = 0; i < length; ++i) {
    reverbed = (size_t)(i + delaySamples);
    if (reverbed > length-1) continue;
    clone[reverbed] += (uint_fast32_t)(pixels[i] * decay);
    // pixels[reverbed] += pixels[i] * decay;
  }

}






void copyBlocks (uint_fast32_t *pixels, uint_fast32_t *clone, size_t w, size_t h, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh)
 {
  size_t i = 0, n, o, sIndex, dIndex;
  std::vector<uint_fast32_t> shape ( sw * sh);
  for (n = 0; n < sh; ++n) {
    for (o = 0; o < sw; ++o) {
      sIndex = (sx + n) * w + (sy + o);
      shape[i] = pixels[sIndex];
      ++i;
    }
  }
  i = 0;
  for (n = 0; n < dh; ++n) {
    for (o = 0; o < dw; ++o) {
      dIndex = (dx + n) * w + (dy + o);
      clone[dIndex] = shape[i];
      ++i;
    }
  }
}

void copyCircles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t w, size_t h, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh, size_t radius)
 {

  size_t shp = 0, total, sIndex, dIndex, scx, scy,i, j;
  int_least32_t circ = radius*radius, cx = 0, cy = 0;
  scx = (size_t)(sh/2);
  scy = (size_t)(sw/2);
  total = w * h;
  std::vector<uint_fast32_t> shape (sh * sw);
  for (i = 0; i < sh; ++i) {
    for (j = 0; j < sw; ++j) {
      cx = i - scx ;
      cy = j - scy ;
      if ( ( cx * cx + cy * cy ) <=  circ  ) {
        sIndex = (sx + i)  * w + (sy + j);
        shape[shp] = pixels[sIndex];
        ++shp;
      }
    }
  }

  shp = 0;
  for (i = 0; i < dh; ++i) {
    for (j = 0; j < dw; ++j) {
      cx = scx - i;
      cy = scy - j;
      if ( (cx * cx + cy * cy) <= circ ) {
        dIndex = (dx + i) * w + (dy + j);
        if (dIndex > total) 
        clone[dIndex] = shape[shp];
        ++shp;
      }
    }
  }
}


void copyTriangles(uint_fast32_t *pixels, uint_fast32_t *clone, size_t w, size_t sx, size_t sy, size_t sw, size_t sh, size_t dx, size_t dy, size_t dw, size_t dh)
{
  // size_t shp = 0, sIndex, dIndex, qt, bs, be, scx, scy, cx, cy,i, j;
  // size_t area = (size_t)(.5 * sh * sw);
  size_t shp = 0, sIndex, dIndex, qt, bs, be, scx, i, j;
  std::vector<uint_fast32_t> shape (sh * sw);
  scx = (size_t)(sh/2);
  // scy = (size_t)(sw/2);
  for (i = 0; i < sh; ++i) {
    qt = ceil(i/2);
    bs = scx - qt;
    be = scx + qt;
    for (j = 0; j < sw; ++j) {
      if ( j > bs && j < be ) {
        sIndex = (sx + i)  * w + (sy + j);
        shape[shp] = pixels[sIndex];
        ++shp;
      }
    }
  }
  shp = 0;
  for (i = 0; i < dh; ++i) {
    qt = ceil(i/2);
    bs = scx - qt;
    be = scx + qt;
    for (j = 0; j < dw; j++) {
      if ( j > bs && j < be ) {
        dIndex = (dx + i) * w + (dy + j);
        clone[dIndex] = shape[shp];
        ++shp;
      }
    }
  }
}


// Using copies
void outtashape (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 

  // size_t circ = radius*radius, shp = 0, cx = 0, cy = 0, n = 0, o = 0, qt, bs, be;
  // double depth = opts->depth;
  size_t length = opts->length, xspace = opts->xspace, yspace = opts->yspace;
  size_t h = opts->height, w = opts->width, x, y, i, j; 
  size_t size = (std::uint_least32_t)xspace * (std::uint_least32_t)yspace;
  size_t sIndex, dIndex, scx, scy, radius = (size_t)(size/2);
  size_t circ = radius*radius, shp = 0, cx = 0, cy = 0, n = 0, o = 0;
  scx = (size_t)(size/2);
  scy = (size_t)(size/2);
  if (size * size > length || size == 0) size = 1;
  std::vector<uint_fast32_t> shape ( size * size);
  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = rand() % (h-1);
      j = rand() % (w-1);

      // i = (size_t)( abs(sin(x)) * h - 1 );
      // j = (size_t)( abs(cos(y)) * w - 1 );

      // prevent overflow
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;

      // copyBlocks(pixels, clone, opts->width, i, j, size, size, x, y, size, size);
      shp = 0;
      // std::vector<uint_fast32_t> shape ( size * size);
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          sIndex = (i + n) * w + (j + o);
          // if (sIndex > length) sIndex = (sIndex - (sIndex - length) - ( i * w + j ) );
          shape[shp] = pixels[sIndex];
          ++shp;
        }
      }

      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          dIndex = (x + n) * w + (y + o);
          // if (dIndex > length) dIndex = (dIndex - (dIndex - length) - ( n * w + o ) );
          clone[dIndex] = shape[shp];
          ++shp;
        }
      }
    }
  }

  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = rand() % (h-1);
      j = rand() % (w-1);
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;


      // std::vector<uint_fast32_t> shape (size * size);
      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          cx = n - scx ;
          cy = o - scy ;
          if ( ( cx * cx + cy * cy ) <=  circ  ) {
            sIndex = (i + n)  * w + (j + o);
            shape[shp] = pixels[sIndex];
            ++shp;
          }
        }
      }
      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          cx = scx - n;
          cy = scy - o;
          if ( (cx * cx + cy * cy) <= circ ) {
            dIndex = (x + n) * w + (y + o);
            clone[dIndex] = shape[shp];
            ++shp;
          }
        }
      }
    }
  }
  
  i = rand() % (h-1);
  j = rand() % (w-1);
  if ( (i+size) > h) i = (i - (i-h)) - size;
  if ( (j+size) > w) j = (j - (j-h)) - size;

  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = (size_t)( rand() % h - 1 );
      j = (size_t)( rand() % w - 1 );
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;

      copyTriangles(pixels, clone, w, i,j, size, size, x, y, size, size);

      // std::vector<uint_fast32_t> shape (size * size);
    //   for (n = 0; n < size; ++n) {
    //     qt = ceil(n/2);
    //     bs = scx - qt;
    //     be = scx + qt;
    //     for (o = 0; o < size; ++o) {
    //       if ( o > bs && o < be ) {
    //         sIndex = (i + n)  * w + (j + o);
    //         shape[shp] = pixels[sIndex];
    //         ++shp;
    //       }
    //     }
    //   }
    //   shp = 0;
    //   for (n = 0; n < size; ++n) {
    //     qt = ceil(n/2);
    //     bs = scx - qt;
    //     be = scx + qt;
    //     for (o = 0; o < size; o++) {
    //       if ( o > bs && o < be ) {
    //         dIndex = (x + n) * w + (y + o);
    //         clone[dIndex] = shape[shp];
    //         ++shp;
    //       }
    //     }
    //   }
    }
  }

}



// Using copies
void outtasquare (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 

  // scx = (size_t)(size/2);
  // scy = (size_t)(size/2);
  // double depth = opts->depth;
  // size_t sIndex, dIndex, scx, scy, radius = (size_t)(size/2);
  size_t length = opts->length, xspace = (std::uint_least32_t)opts->xspace, yspace = (std::uint_least32_t)opts->yspace;
  size_t h = opts->height, w = opts->width, x, y, i, j; 
  size_t size = xspace * yspace;
  size_t sIndex, dIndex; //, scx, scy, radius = (size_t)(size/2);
  size_t shp = 0, n = 0, o = 0;
  if (size * size > length || size == 0) size = 1;
  std::vector<uint_fast32_t> shape ( size * size);
  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = rand() % (h-1);
      j = rand() % (w-1);

      // i = (size_t)( abs(sin(x)) * h - 1 );
      // j = (size_t)( abs(cos(y)) * w - 1 );

      // prevent overflow
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;

      // copyBlocks(pixels, clone, opts->width, i, j, size, size, x, y, size, size);
      shp = 0;
      // std::vector<uint_fast32_t> shape ( size * size);
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          sIndex = (i + n) * w + (j + o);
          // if (sIndex > length) sIndex = (sIndex - (sIndex - length) - ( i * w + j ) );
          shape[shp] = pixels[sIndex];
          ++shp;
        }
      }

      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          dIndex = (x + n) * w + (y + o);
          // if (dIndex > length) dIndex = (dIndex - (dIndex - length) - ( n * w + o ) );
          clone[dIndex] = shape[shp];
          ++shp;
        }
      }
    }
  }
}


// Using copies
void outtatriangle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 

  size_t length = opts->length, xspace = (std::size_t)opts->xspace, yspace = (std::size_t)opts->yspace;
  size_t h = opts->height, w = opts->width, x, y, i, j; 
  size_t size = xspace * yspace;
  // size_t scx, scy; //, radius = (size_t)(size/2);
  // size_t circ = radius*radius, shp = 0, cx = 0, cy = 0, n = 0, o = 0, qt, bs, be;
  // scx = (size_t)(size/2);
  // scy = (size_t)(size/2);
  // double depth = opts->depth;
  if (size * size > length || size == 0) size = 1;
  std::vector<uint_fast32_t> shape ( size * size);
  i = rand() % (h-1);
  j = rand() % (w-1);
  if ( (i+size) > h) i = (i - (i-h)) - size;
  if ( (j+size) > w) j = (j - (j-h)) - size;

  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = (size_t)( rand() % h - 1 );
      j = (size_t)( rand() % w - 1 );
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;

      copyTriangles(pixels, clone, w, i,j, size, size, x, y, size, size);


    }
  }

}

// Using copies
void outtacircle (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{ 

  size_t length = opts->length, xspace = (std::size_t)opts->xspace, yspace = (std::size_t)opts->yspace;
  size_t h = opts->height, w = opts->width, x, y, i, j; 
  size_t size = xspace * yspace;
  size_t sIndex, dIndex, scx, scy, radius = (size_t)(size/2);
  size_t circ = radius*radius, shp = 0, cx = 0, cy = 0, n = 0, o = 0; //, qt, bs, be;
  // double depth = opts->depth;
  scx = (size_t)(size/2);
  scy = (size_t)(size/2);
  std::vector<uint_fast32_t> shape ( size * size);
  if (size * size > length || size == 0) size = 1;
  std::cout << "Size:   " << size << std::endl;
  std::cout << "Xspace: " << (uint_least32_t)xspace << std::endl;
  std::cout << "Yspace: " << (uint_least32_t)yspace << std::endl;
  std::cout << "Length: " << (uint_least32_t)length << std::endl;

  for (x = 0; (x+size) < h; x+=size) {
    for (y = 0; (y+size) < w; y+=size) {
      i = rand() % (h-1);
      j = rand() % (w-1);
      if ( (i+size) > h) i = (i - (i-h)) - size;
      if ( (j+size) > w) j = (j - (j-h)) - size;
      if (i > 0 || i == 0) i = 1;
      if (j > 0 || j == 0) j = 1;

      // std::vector<uint_fast32_t> shape (size * size);
      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          cx = n - scx ;
          cy = o - scy ;
          if ( ( cx * cx + cy * cy ) <=  circ  ) {
            sIndex = (i + n)  * w + (j + o);
            shape[shp] = pixels[sIndex];
            ++shp;
          }
        }
      }
      shp = 0;
      for (n = 0; n < size; ++n) {
        for (o = 0; o < size; ++o) {
          cx = scx - n;
          cy = scy - o;
          if ( (cx * cx + cy * cy) <= circ ) {
            dIndex = (x + n) * w + (y + o);
            clone[dIndex] = shape[shp];
            ++shp;
          }
        }
      }
    }
  }
 
}



void pssht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  size_t h = opts->height, w = opts->width, index, x, y;
  // bad, I know, but I need the negative
  // magic numbers
  uint_fast32_t u32c;
  uint_fast8_t r, g, b, /*fr,*/ fg, /*fb,*/ sr, /*sg,*/ /*sb,*/ /*tr, tg,*/ tb;
  
  if (isLSB()) { 
    for(x = 0; x < h; ++x) {
      for (y = 0; y < w; ++y) {
        index = x * w + y;
        u32c = pixels[index];
        UNPACKLSBUINT32COLOR(r, g, b, u32c);
        
        // reusing same variable 
        u32c = r+g+b; 

        if (y % (w-1) == 0 ) {
          clone[index] = pixels[index];
          continue;
        }
        if (u32c > 100 && (y > 20) && (y + 22) < w ) { // purple
        // Purple - Yellow

          UNPACKLSBUINT32COLOR(r, fg, b, (pixels[index - 19]))
          UNPACKLSBUINT32COLOR(sr, g, b, (pixels[index -  4]))
          UNPACKLSBUINT32COLOR(r, g, tb, (pixels[index + 20]))

          r = (uint_fast8_t)(( fg + sr ) / 2);
          g = (uint_fast8_t)(( tb + fg ) / 2);
          b = (uint_fast8_t)(( sr + tb ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);
        
        } else {

          clone[index] = (uint_fast32_t)((pixels[index] + pixels[index]) / 2);
        }
      }
    }
  } 
}


void posht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  size_t h = opts->height, w = opts->width, /*length = opts->length,*/ index, x, y;
  // bad, I know, but I need the negative
  // magic numbers
  uint_fast32_t u32c;
  uint_fast8_t r, g, b, fr, fg, fb, sr, sg, /*sb,*/ /*tr,*/ tg, tb;
  
  if (isLSB()) { 
    for(x = 0; x < h; ++x) {
      for (y = 0; y < w; ++y) {
        index = x * w + y;
        u32c = pixels[index];
        UNPACKLSBUINT32COLOR(r, g, b, u32c);
        
        // reusing same variable 
        u32c = r+g+b; 

        if (y % (w-1) == 0 ) {
          clone[index] = pixels[index];
          continue;
        }
        if (u32c > 100 && (y > 20) && (y + 22) < w ) { // purple
        // Purple - Yellow

          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index - 19]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index -  4]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index + 20]))

          r = (uint_fast8_t)(( fg + sr ) / 2);
          g = (uint_fast8_t)(( tb + fg ) / 2);
          b = (uint_fast8_t)(( sr + tb ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);
        
        } else if (u32c > 50 && (y > 15) && (y + 16) < w ){
          // cyan-red

          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index - 13]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index + 15]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index +  4]))

          r = (uint_fast8_t)(( fb + fb ) / 2);
          g = (uint_fast8_t)(( sr + tg ) / 2);
          b = (uint_fast8_t)(( tg + sr ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);

        } else if ( (y > 10) && (y + 10) < w ) {
          // Orange - blue

          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index +  4]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index - 10]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index +  7]))

          r = (uint_fast8_t)(( tb + fr ) / 2);
          g = (uint_fast8_t)(( fr + tb ) / 2);
          b = (uint_fast8_t)(( sg + sg ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);      
        } else {

          clone[index] = (uint_fast32_t)((pixels[index] + pixels[index]) / 2);
        }
      }
    }
  } 
}





void losht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  size_t h = opts->height, w = opts->width, /*length = opts->length,*/ index, x, y;
  // bad, I know, but I need the negative
  // (I dont know what the above comment was for...lol)
  // magic numbers
  uint_fast32_t u32c;
  uint_fast8_t r, g, b, fr, fg, fb, sr, sg, sb, tr, tg, tb;
  
  if (isLSB()) { 
    for(x = 0; x < h; ++x) {
      for (y = 0; y < w; ++y) {
        index = x * w + y;
        u32c = pixels[index];
        UNPACKLSBUINT32COLOR(r, g, b, u32c);
        
        // reusing same variable 
        u32c = r+g+b; 

        if (y % (w-1) == 0 ) {
          clone[index] = pixels[index];
          continue;
        }
        if (u32c > 100 && (y > 20) && (y + 22) < w ) { 

          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index -  4]))
          UNPACKLSBUINT32COLOR(sr, sg, sb, (pixels[index - 19]))
          UNPACKLSBUINT32COLOR(tr, tg, tb, (pixels[index + 20]))

          r = (uint_fast8_t)(( sg + tr ) / 2);
          g = (uint_fast8_t)(( tb + fg ) / 2);
          b = (uint_fast8_t)(( fr + sb ) / 2);
 
          clone[index] = LSBUINT32COLOR(r,g,b);
        
        } else if (u32c > 50 && (y > 15) && (y + 16) < w ){

          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index - 13]))
          UNPACKLSBUINT32COLOR(sr, sg, sb, (pixels[index - 15]))
          UNPACKLSBUINT32COLOR(tr, tg, tb, (pixels[index +  4]))

          r = (uint_fast8_t)(( fb + sb ) / 2);
          g = (uint_fast8_t)(( sr + tg ) / 2);
          b = (uint_fast8_t)(( tg + sr ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);

        } else if ( (y > 10) && (y + 10) < w ) {
          // Orange - blue
          UNPACKLSBUINT32COLOR(fr, fg, fb, (pixels[index +  4]))
          UNPACKLSBUINT32COLOR(sr, sg, sb, (pixels[index - 10]))
          UNPACKLSBUINT32COLOR(tr, tg, tb, (pixels[index +  7]))

          r = (uint_fast8_t)(( tb + fr ) / 2);
          g = (uint_fast8_t)(( fr + sb ) / 2);
          b = (uint_fast8_t)(( sg + tg ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);      
        } else {

          clone[index] = (uint_fast32_t)((pixels[index] + pixels[index]) / 2);
        }
      }
    }
  } 
}






void fosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{

  size_t h = opts->height, w = opts->width, index, x, y;
  uint_fast32_t u32c;
  uint_fast8_t r, g, b, fr, /*fg,*/ fb, sr, sg, /*sb,*/ /*tr,*/ tg, tb;
  
  if (isLSB()) { 
    for(x = 0; x < h; ++x) {
      for (y = 0; y < w; ++y) {
        index = x * w + y;
        u32c = pixels[index];
        UNPACKLSBUINT32COLOR(r, g, b, u32c);
        
        // reusing same variable 
        u32c = r+g+b; 

        if (y % (w-1) == 0 ) {
          clone[index] = pixels[index];
          continue;
        }
        if (u32c > 100 && (y > 20) && (y + 22) < w ) { 

          UNPACKLSBUINT32COLOR(fr, g, fb, (pixels[index -  4]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index - 19]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index + 20]))

          r = (uint_fast8_t)(( sg + fr ) / 2);
          g = (uint_fast8_t)(( tb + sg ) / 2);
          b = (uint_fast8_t)(( fr + tb ) / 2);
 
          clone[index] = LSBUINT32COLOR(r,g,b);
        
        } else if (u32c > 50 && (y > 15) && (y + 16) < w ){

          UNPACKLSBUINT32COLOR(fr, g, fb, (pixels[index - 13]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index - 15]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index +  4]))

          r = (uint_fast8_t)(( fb + tg ) / 2);
          g = (uint_fast8_t)(( sr + fb ) / 2);
          b = (uint_fast8_t)(( tg + sr ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);

        } else if ( (y > 10) && (y + 10) < w ) {
          // Orange - blue
          UNPACKLSBUINT32COLOR(fr, g, fb, (pixels[index +  4]))
          UNPACKLSBUINT32COLOR(sr, sg, b, (pixels[index - 10]))
          UNPACKLSBUINT32COLOR(r, tg, tb, (pixels[index +  7]))


          r = (uint_fast8_t)(( tb + sg ) / 2);
          g = (uint_fast8_t)(( fr + tb ) / 2);
          b = (uint_fast8_t)(( sg + fr ) / 2);

          clone[index] = LSBUINT32COLOR(r,g,b);      
        } else {

          clone[index] = (uint_fast32_t)((pixels[index] + pixels[index]) / 2);
        }
      }
    }
  } 
}






void source_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // const size_t length = opts->length, h = opts->height;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4, sIndex; 
        // movement kernel
        // var m = [
        //   index-((w*4) + 4), index-(w*4), index-(w*4) + 4,
        //   index-4, index, index+4,
        //   index+((w*4) -4), index+ (w*4), index+(w*4) + 4
        // ];
        // var ml = m.length;
        uint_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t cx = 0; cx < 3; ++cx) {
          for (size_t cy = 0; cy < 3; ++cy) {
            sIndex = ((x + cx) * w + (y + cy)) * 4;
          avgRed +=   chunk[ sIndex + 0 ] - dupBuffer[ sIndex + 0];
          avgBlue +=  chunk[ sIndex + 1 ] - dupBuffer[ sIndex + 1];
          avgGreen += chunk[ sIndex + 2 ] - dupBuffer[ sIndex + 2];

          }
        }

        dupBuffer[index+0] = chunk[index+0] - (uint_fast8_t)(avgRed   / 9);
        dupBuffer[index+1] = chunk[index+1] - (uint_fast8_t)(avgBlue  / 9);
        dupBuffer[index+2] = chunk[index+2] - (uint_fast8_t)(avgGreen / 9);

    }
  }
}



void source_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // const size_t length = opts->length, h = opts->height;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4, sIndex; 
        // movement kernel
        // var m[9] = {
        //   index-((w*4) + 4), index-(w*4), index-(w*4) + 4,
        //   index-4, index, index+4,
        //   index+((w*4) -4), index+ (w*4), index+(w*4) + 4
        // };
        // var ml = m.length;
        uint_fast16_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t cx = 0; cx < 3; ++cx) {
          for (size_t cy = 0; cy < 3; ++cy) {
            sIndex = ((x + cx) * w + (y + cy)) * 4;
            avgRed +=   chunk[ sIndex     ] - dupBuffer[ sIndex    ];
            avgBlue +=  chunk[ sIndex + 1 ] - dupBuffer[ sIndex + 1];
            avgGreen += chunk[ sIndex + 2 ] - dupBuffer[ sIndex + 2];

          }
        }

        dupBuffer[index+0] = (chunk[index  ] + (uint_fast8_t)(avgRed   / 9)/2);
        dupBuffer[index+1] = (chunk[index+1] + (uint_fast8_t)(avgBlue  / 9)/2);
        dupBuffer[index+2] = (chunk[index+2] + (uint_fast8_t)(avgGreen / 9)/2);

    }
  }
}




void sample_poisson_subtract_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // const size_t length = opts->length, h = opts->height;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4, sIndex; 
        // movement kernel
        // var m = [
        //   index-((w*4) + 4), index-(w*4), index-(w*4) + 4,
        //   index-4, index, index+4,
        //   index+((w*4) -4), index+ (w*4), index+(w*4) + 4
        // ];
        // var ml = m.length;
        uint_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        for (size_t cx = 0; cx < 3; ++cx) {
          for (size_t cy = 0; cy < 3; ++cy) {
            sIndex = ((x + cx) * w + (y + cy)) * 4;
          avgRed +=   chunk[ sIndex + 0 ] - dupBuffer[ sIndex + 0];
          avgBlue +=  chunk[ sIndex + 1 ] - dupBuffer[ sIndex + 1];
          avgGreen += chunk[ sIndex + 2 ] - dupBuffer[ sIndex + 2];

          }
        }

        dupBuffer[index+0] = dupBuffer[index+0] - (uint_fast8_t)(avgRed   / 9);
        dupBuffer[index+1] = dupBuffer[index+1] - (uint_fast8_t)(avgBlue  / 9);
        dupBuffer[index+2] = dupBuffer[index+2] - (uint_fast8_t)(avgGreen / 9);

    }
  }
}



void sample_poisson_add_8_bit (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) 
{
  // const size_t length = opts->length, h = opts->height;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  std::vector<size_t> m(9);
  size_t ml = 9;
  // m.reserve(ml);
  
   for (size_t x=0,x1=1, h1 = opts->height-1; x1 < h1; ++x, ++x1) {
    for (size_t y=0,y1=1,w = opts->width, w1 = opts->width-1; y1 < w1; ++y, ++y1) {
        // starting at 1,1
        size_t index = (x1*w+y1)*4;//, sIndex; 
        // movement kernel
        m[0] = index-((w*4) + 4);
        m[1] = index-(w*4);
        m[2] = index-(w*4) + 4;
        m[3] = index-4;
        m[4] = index;
        m[5] = index+4;
        m[6] = index+((w*4) -4);
        m[7] = index+ (w*4);
        m[8] = index+(w*4) + 4;
 
        int_least32_t avgRed = 0, avgBlue = 0, avgGreen = 0;

        // for (size_t cx = 0; cx < 3; ++cx) {
        //   for (size_t cy = 0; cy < 3; ++cy) {
            // sIndex = ((x + cx) * w + (y + cy)) * 4;
        for (size_t k = 0; k < ml; ++k) {

            avgRed +=   chunk[ m[k] + 0 ] - dupBuffer[ m[k] + 0];
            avgBlue +=  chunk[ m[k] + 1 ] - dupBuffer[ m[k] + 1];
            avgGreen += chunk[ m[k] + 2 ] - dupBuffer[ m[k] + 2];
        }

          // }
        // }
        avgRed   /= 9;
        avgGreen /= 9;
        avgBlue  /= 9;
        avgRed   += chunk[index +0];       
        avgGreen += chunk[index +1];       
        avgBlue  += chunk[index +2];       

        dupBuffer[index+0] =  avgRed; // (avgRed   > 254) ? avgRed - 255   : avgRed;
        dupBuffer[index+1] =  avgGreen; // (avgGreen > 254) ? avgGreen - 255 : avgGreen;
        dupBuffer[index+2] =  avgBlue; // (avgBlue  > 254) ? avgBlue - 255  : avgBlue;

    }
  }
}



void smosh (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{

  size_t h = opts->height, w = opts->width,index,x,y;
  uint_fast32_t u32c, u32v;
  for( x = 0; x < h; x++) {
    for (y = 0; y < w; y++) {
      index = x * w +y;
      u32c = pixels[index];
      u32v = (u32c &0xff) + ((u32c >> 8)&0xff) + ((u32c >> 16)&0xff);
      
      if (y % (w-1) == 0 ) {
        continue;
      }
      if (u32v > 100 && (y > 20) && (y + 21) < w ) {
        clone[index] = (255 << 24) | (((pixels[index - 19] >> 8)&0xFF)  << 16) | (((pixels[index + 20] >> 16)&0xFF) << 8)| (pixels[index -  4]&0xFF);
      } else if (u32v > 50 && (y > 15) && (y + 16) < w ){
        clone[index] = (255 << 24) | (((pixels[index - 13] >> 16)&0xFF) << 16) | ((pixels[index + 15] &0xFF) <<  8)| ((pixels[index +  4] >> 8)&0xFF);
       } else if ((y > 10) && (y + 10) < w ) {
        clone[index] = (255 << 24) | (((pixels[index +  7] >> 16)&0xFF) << 16) | ((pixels[index + 4] &0xFF)  <<  8)| ((pixels[index -   10] >> 8)&0xFF);
      } else {
        clone[index] = pixels[index];
      }
    }
  }
}




void smosht (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{
  // const size_t length = opts->length;
  uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
  uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
  size_t h = opts->height, w = opts->width,index, x, y;
  uint_fast32_t u32v;

  for( x = 0; x < h; x++) {
    for( y = 0; y < w; y++) {
       index = (x*w+y)*4;
       u32v = chunk[index] + chunk[index+1] + chunk[index+2];
      if (y % (w-1) == 0 ) {
        dupBuffer[index+0] = chunk[index+0];
        dupBuffer[index+1] = chunk[index+1];
        dupBuffer[index+2] = chunk[index+2];
        dupBuffer[index+3] = 255;
        continue;
      }
      if (u32v > 100 && (y > 20) && ( y + 21) < w ) {
        dupBuffer[ index ] = chunk[index - (76)+1];
        dupBuffer[index+1] = chunk[index + (80)+2];
        dupBuffer[index+2] = chunk[index - (16)+0];
        dupBuffer[index+3] = 255; 
      } else if (u32v > 50 && (y > 15) && (y + 16) < w ){
        dupBuffer[ index ] = chunk[index - (52)+2];
        dupBuffer[index+1] = chunk[index + (60)+0];
        dupBuffer[index+2] = chunk[index + (16)+1];
        dupBuffer[index+3] = 255; 
      } else if (  (y > 10) && (y + 10) < w ) {
        dupBuffer[ index ] = chunk[index + (28)+2];
        dupBuffer[index+1] = chunk[index + (16)+0];
        dupBuffer[index+2] = chunk[index - (40)+1];
        dupBuffer[index+3] = 255; 
      } else {
        dupBuffer[ index ] = chunk[ index ];
        dupBuffer[index+1] = chunk[index+1];
        dupBuffer[index+2] = chunk[index+2];
        dupBuffer[index+3] = chunk[index+3];
      }
    }
  }

}



void gradient_blend_add_sample (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{
// const size_t length = opts->length, h = opts->height;
uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
size_t w = opts->width,h1 = opts->height-1, w1 = opts->width-1, index, /*sIndex, */ x, y, x1, y1;
uint_least8_t ml = 9;
std::vector<size_t> m(ml);
int avgRed = 0, avgBlue = 0, avgGreen = 0;
for (x=0,x1=1; x1 < h1; x++, x1++) {
  for (y=0,y1=1; y1 < w1; y++, y1++) {
    // starting at 1,1
    index = (x1*w+y1)*4; 
    // movement kernel
    // movement kernel
    m[0] = index-((w*4) + 4);
    m[1] = index-(w*4);
    m[2] = index-(w*4) + 4;
    m[3] = index-4;
    m[4] = index;
    m[5] = index+4;
    m[6] = index+((w*4) -4);
    m[7] = index+ (w*4);
    m[8] = index+(w*4) + 4;
    avgRed = 0, avgBlue = 0, avgGreen = 0;
    for (size_t k = 0; k < ml; k++) {
      avgRed +=   chunk[ (m[k]) + 0 ] - dupBuffer[ (m[k]) + 0];
      avgBlue +=  chunk[ (m[k]) + 1 ] - dupBuffer[ (m[k]) + 1];
      avgGreen += chunk[ (m[k]) + 2 ] - dupBuffer[ (m[k]) + 2];
    }

    // console.log("Red: %d,Green: %d,Blue: %d" ,avgRed, avgGreen, avgBlue);
    // console.log("Added Red: %d, Added Green: %d, Added Blue: %d" ,(chunk[index+0] + (avgRed   / ml)), (chunk[index+1] + (avgGreen   / ml)), (chunk[index+2] + (avgBlue   / ml)));
    // console.log("Before Red: %d,Before Green: %d, Before Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);
    
    dupBuffer[index+0] = dupBuffer[index+0] + (avgRed   / ml);
    dupBuffer[index+1] = dupBuffer[index+1] + (avgBlue  / ml);
    dupBuffer[index+2] = dupBuffer[index+2] + (avgGreen / ml);
    
    // console.log("After Red: %d,After Green: %d, After Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);

  }
}
}



void gradient_blend_subtract_sample (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{
// const size_t length = opts->length, h = opts->height;
uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
size_t w = opts->width,h1 = opts->height-1, w1 = opts->width-1, index, x, y, x1, y1;
uint_least8_t ml = 9;
std::vector<size_t> m(ml);
int avgRed = 0, avgBlue = 0, avgGreen = 0;
for (x=0,x1=1; x1 < h1; x++, x1++) {
  for (y=0,y1=1; y1 < w1; y++, y1++) {
    // starting at 1,1
    index = (x1*w+y1)*4;
    // movement kernel
    // movement kernel
    m[0] = index-((w*4) + 4);
    m[1] = index-(w*4);
    m[2] = index-(w*4) + 4;
    m[3] = index-4;
    m[4] = index;
    m[5] = index+4;
    m[6] = index+((w*4) -4);
    m[7] = index+ (w*4);
    m[8] = index+(w*4) + 4;
    avgRed = 0, avgBlue = 0, avgGreen = 0;
    for (size_t k = 0; k < ml; k++) {
      avgRed +=   chunk[ (m[k]) + 0 ] - dupBuffer[ (m[k]) + 0];
      avgBlue +=  chunk[ (m[k]) + 1 ] - dupBuffer[ (m[k]) + 1];
      avgGreen += chunk[ (m[k]) + 2 ] - dupBuffer[ (m[k]) + 2];
    }

    // console.log("Red: %d,Green: %d,Blue: %d" ,avgRed, avgGreen, avgBlue);
    // console.log("Added Red: %d, Added Green: %d, Added Blue: %d" ,(chunk[index+0] + (avgRed   / ml)), (chunk[index+1] + (avgGreen   / ml)), (chunk[index+2] + (avgBlue   / ml)));
    // console.log("Before Red: %d,Before Green: %d, Before Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);
    
    dupBuffer[index+0] = dupBuffer[index+0] - (avgRed   / ml);
    dupBuffer[index+1] = dupBuffer[index+1] - (avgBlue  / ml);
    dupBuffer[index+2] = dupBuffer[index+2] - (avgGreen / ml);
    
    // console.log("After Red: %d,After Green: %d, After Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);

  }
}
}




void gradient_blend_subtract_source (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{
// const size_t length = opts->length;
uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
size_t w = opts->width,h1 = opts->height-1, w1 = opts->width-1, index, x, y, x1, y1;
uint_least8_t ml = 9;
std::vector<size_t> m(ml);
int avgRed = 0, avgBlue = 0, avgGreen = 0;
for (x=0,x1=1; x1 < h1; x++, x1++) {
  for (y=0,y1=1; y1 < w1; y++, y1++) {
    // starting at 1,1
    index = (x1*w+y1)*4; 
    // movement kernel
    // movement kernel
    m[0] = index-((w*4) + 4);
    m[1] = index-(w*4);
    m[2] = index-(w*4) + 4;
    m[3] = index-4;
    m[4] = index;
    m[5] = index+4;
    m[6] = index+((w*4) -4);
    m[7] = index+ (w*4);
    m[8] = index+(w*4) + 4;
    avgRed = 0, avgBlue = 0, avgGreen = 0;
    for (size_t k = 0; k < ml; k++) {
      avgRed +=   chunk[ (m[k]) + 0 ] - dupBuffer[ (m[k]) + 0];
      avgBlue +=  chunk[ (m[k]) + 1 ] - dupBuffer[ (m[k]) + 1];
      avgGreen += chunk[ (m[k]) + 2 ] - dupBuffer[ (m[k]) + 2];
    }

    // console.log("Red: %d,Green: %d,Blue: %d" ,avgRed, avgGreen, avgBlue);
    // console.log("Added Red: %d, Added Green: %d, Added Blue: %d" ,(chunk[index+0] + (avgRed   / ml)), (chunk[index+1] + (avgGreen   / ml)), (chunk[index+2] + (avgBlue   / ml)));
    // console.log("Before Red: %d,Before Green: %d, Before Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);
    
    dupBuffer[index+0] = chunk[index+0] - (avgRed   / ml);
    dupBuffer[index+1] = chunk[index+1] - (avgBlue  / ml);
    dupBuffer[index+2] = chunk[index+2] - (avgGreen / ml);
    
    // console.log("After Red: %d,After Green: %d, After Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);

  }
}
}


void noop  (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts) {

}


void gradient_blend_add_source (uint_fast32_t *pixels, uint_fast32_t *clone, Options *opts)
{

// const size_t length = opts->length;
// if ( (length * 4) % 4 != 0 ) return 1;
uint_least8_t *chunk  = reinterpret_cast<uint_least8_t*>(pixels);
uint_least8_t *dupBuffer = reinterpret_cast<uint_least8_t*>(clone);
size_t w = opts->width,h1 = opts->height-1, w1 = opts->width-1, index, x, y, x1, y1;
uint_least8_t ml = 9;
std::vector<size_t> m(ml);
int avgRed = 0, avgBlue = 0, avgGreen = 0;
for (x=0,x1=1; x1 < h1; x++, x1++) {
  for (y=0,y1=1; y1 < w1; y++, y1++) {
    // starting at 1,1
    index = (x1*w+y1)*4; 
    // movement kernel
    // movement kernel
    m[0] = index-((w*4) + 4);
    m[1] = index-(w*4);
    m[2] = index-(w*4) + 4;
    m[3] = index-4;
    m[4] = index;
    m[5] = index+4;
    m[6] = index+((w*4) -4);
    m[7] = index+ (w*4);
    m[8] = index+(w*4) + 4;
    avgRed = 0, avgBlue = 0, avgGreen = 0;
    for (size_t k = 0; k < ml; k++) {
      avgRed +=   chunk[ (m[k]) + 0 ] - dupBuffer[ (m[k]) + 0];
      avgBlue +=  chunk[ (m[k]) + 1 ] - dupBuffer[ (m[k]) + 1];
      avgGreen += chunk[ (m[k]) + 2 ] - dupBuffer[ (m[k]) + 2];
    }

    // console.log("Red: %d,Green: %d,Blue: %d" ,avgRed, avgGreen, avgBlue);
    // console.log("Added Red: %d, Added Green: %d, Added Blue: %d" ,(chunk[index+0] + (avgRed   / ml)), (chunk[index+1] + (avgGreen   / ml)), (chunk[index+2] + (avgBlue   / ml)));
    // console.log("Before Red: %d,Before Green: %d, Before Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);
    
    dupBuffer[index+0] = chunk[index+0] + (avgRed   / ml);
    dupBuffer[index+1] = chunk[index+1] + (avgBlue  / ml);
    dupBuffer[index+2] = chunk[index+2] + (avgGreen / ml);
    
    // console.log("After Red: %d,After Green: %d, After Blue: %d" ,chunk[index+0], chunk[index+1], chunk[index+2]);

  }
}
}






void funnelEffect(std::uint_fast32_t *pixels, std::uint_fast32_t *clone, Options *opts ) 
{

/*

  Restore code goes here

*/

/*

  Channel selector can also go here
*/
  channelSelector32LSB(pixels, clone, opts);

  switch (opts->effect) {
    // ZOOMS
    case 'a' :  
      zoomth(pixels, clone, opts);
      break;
    case 'b' :  
      zoom_b(pixels, clone, opts);
      break;
    case 'c' :  
      zoom_r(pixels, clone, opts);
      break;
    case 'd' :  
      crunch(pixels, clone, opts);
      break;
    // SHAPES
    case 'e' :  
      pixelate(pixels, clone, opts);
      break;
    case 'f' :  
      outtashape(pixels, clone, opts);
      break;
    case 'g' :  
      outtacircle(pixels, clone, opts);
      break;
    case 'h' :  
      outtasquare(pixels, clone, opts);
      break;
    case 'i' :  
      outtatriangle(pixels, clone, opts);
      break;
     // COLORSWAP
    case 'j' :  
      rgb_2_bgr(pixels, clone, opts);
      break;
    case 'k' :  
      rgb_2_gbr(pixels, clone, opts);
      break;
    case 'l' :  
      rgb_2_grb(pixels, clone, opts);
      break;
    case 'm' :  
      rgb_2_brg(pixels, clone, opts);
      break;
    // AUIDO
    case 'n' :  
      wet_reverb(pixels, clone, opts);
      break;
    // DISPLACEMENTS
    case 'o' :  
      deplace(pixels, clone, opts);
      break;      
    // PIXELSORTS
    case 'p' :  
      pixelsort(pixels, clone, opts);
      break;
    // SMOSHES
    case 's' :  
      smosht(pixels, clone, opts);
      break;
    case 't' :  
      posht(pixels, clone, opts);
      break;
    case 'u' :  
      fosht(pixels, clone, opts);
      break;
    case 'v' :  
      losht(pixels, clone, opts);
      break;

    // GRADIENT BLEND
    case 'w' :  
      gradient_blend_add_sample(pixels, clone, opts);
      break;
    case 'x' :  
      gradient_blend_add_source(pixels, clone, opts);
      break;
    case 'y' :  
      gradient_blend_subtract_sample(pixels, clone, opts);
      break;
    case 'z' :  
      gradient_blend_subtract_source(pixels, clone, opts);
      break;
    case 'A' :
      sample_poisson_add_8_bit(pixels, clone, opts);
      break;
    case 'B' :
      sample_poisson_subtract_8_bit(pixels, clone, opts);
      break;
    case 'C' : 
      source_poisson_add_8_bit(pixels, clone, opts);
      break;
    case 'D' :
      source_poisson_subtract_8_bit(pixels, clone, opts);
      break;
    case 'N' :
      poisson_blend_add_better_math (pixels, clone, opts);
      break;
    case 'O' :
      poisson_blend_subtract_better_math (pixels, clone, opts);
      break;
    case 'P' :
      poisson_blend_multiply_better_math (pixels, clone, opts);
      break;
    case 'Q' :
      poisson_blend_divide_better_math (pixels, clone, opts);
      break;
    case 'W' :
      poisson_blend_add_sample_void(pixels, clone, opts); 
      break;
    case 'X' :
      poisson_blend_subtract_sample_void(pixels, clone, opts); 
      break;
    case 'Y' :
      poisson_blend_add_source_void(pixels, clone, opts); 
      break;
    case 'V' :
      poisson_blend_subtract_source_void(pixels, clone, opts); 
      break;
    default: 
      noop(pixels, clone, opts);
      break;
  }


}

// void readImageDataFromOpenCv (std::string filename, uint_least8_t *cvpixels, Options *opts)
// {

//   cv::Mat m;
//   m = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
//   if (!m.data) return;
//   (*cvpixels) = m.data;
//   // filname = argv[1];
//   // std::istringstream iss(argv[2]);
//   // if (!(iss >> num)) std::cerr << "Invalid number " << argv[2] << std::endl;
//   // size_t elemSize = m.elemSize();
//   // size_t total = m.total();
//   // size_t length = elemSize * total;

//   setOptions(m, opts);
// }


void getParametersFromNode(v8::Local<v8::Array> fullGStack, Options *stack) 
{
  v8::Local<v8::Value> val;
  v8::Local<v8::Object> instructions;
  v8::Local<v8::Number> num;
  for (std::size_t i = 0; i < fullGStack->Length(); /*runTimes;*/   ++i) {
    std::cout << "its on run # " << i << std::endl;
    if(fullGStack->IsObject()) {
      val = fullGStack->Get(i);
      instructions = val.As<v8::Object>();
      

      Nan::MaybeLocal<v8::Value> effect =  Nan::Get(instructions, Nan::New("effect").ToLocalChecked());
      if ( effect.IsEmpty()) {
        Nan::SyntaxError("Object must have a effect property");
      }
      num = effect.ToLocalChecked().As<v8::Number>();
  
      char intEffect = num->NumberValue();
      std::cout << "Effect is " << (int)intEffect << std::endl;

      stack[i].effect = intEffect;


      Nan::MaybeLocal<v8::Value> order =  Nan::Get(instructions, Nan::New("order").ToLocalChecked());
      if ( order.IsEmpty()) {
        Nan::SyntaxError("Object must have a order property");
      }
      num = order.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intOrder = num->NumberValue();
      // std::cout << "Order is " << intOrder << std::endl;

      stack[i].order = intOrder;

      Nan::MaybeLocal<v8::Value> channel =  Nan::Get(instructions, Nan::New("channel").ToLocalChecked());
      if ( channel.IsEmpty()) {
        Nan::SyntaxError("Object must have a channel property");
      }
      num = channel.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intChan = num->NumberValue();
      // std::cout << "Channel is " << intChan << std::endl;

      stack[i].channel = intChan;

      Nan::MaybeLocal<v8::Value> threshold =  Nan::Get(instructions, Nan::New("threshold").ToLocalChecked());
      if ( threshold.IsEmpty()) {
        Nan::SyntaxError("Object must have a threshold property");
      }
      num = threshold.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intThreshold = num->NumberValue();
      // std::cout << "Threshold is " << intThreshold << std::endl;

      stack[i].threshold = intThreshold;


      Nan::MaybeLocal<v8::Value> depth =  Nan::Get(instructions, Nan::New("depth").ToLocalChecked());
      if ( depth.IsEmpty()) {
        Nan::SyntaxError("Object must have a depth property");
      }
      num = depth.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      double doubleDepth = num->NumberValue();
      // std::cout << "Depth is " << doubleDepth << std::endl;

      stack[i].depth = doubleDepth;


      Nan::MaybeLocal<v8::Value> xspace =  Nan::Get(instructions, Nan::New("xspace").ToLocalChecked());
      if ( xspace.IsEmpty()) {
      // if ( xspace.Isundefined()) {
        Nan::SyntaxError("Object must have a xspace property");
      }
      num = xspace.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intXSpace = num->NumberValue();
      // std::cout << "X-Space is " << intXSpace << std::endl;

      stack[i].xspace = intXSpace;


      Nan::MaybeLocal<v8::Value> yspace =  Nan::Get(instructions, Nan::New("yspace").ToLocalChecked());
      if ( yspace.IsEmpty()) {
        Nan::SyntaxError("Object must have a yspace property");
      }
      num = yspace.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intYSpace = num->NumberValue();
      // std::cout << "Y-Space is " << intYSpace << std::endl;

      stack[i].yspace = intYSpace;


      Nan::MaybeLocal<v8::Value> stroke =  Nan::Get(instructions, Nan::New("stroke").ToLocalChecked());
      if ( stroke.IsEmpty()) {
        Nan::SyntaxError("Object must have a stroke property");
      }
      num = stroke.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intStroke = num->NumberValue();
      // std::cout << "Stroke is " << intStroke << std::endl;

      stack[i].stroke = intStroke;

      Nan::MaybeLocal<v8::Value> weight =  Nan::Get(instructions, Nan::New("weight").ToLocalChecked());
      if ( weight.IsEmpty()) {
        Nan::SyntaxError("Object must have a weight property");
      }
      num = weight.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intWeight = num->NumberValue();
      // std::cout << "Weight is " << intWeight << std::endl;

      stack[i].weight = intWeight;


      Nan::MaybeLocal<v8::Value> color =  Nan::Get(instructions, Nan::New("color").ToLocalChecked());
      if ( color.IsEmpty()) {
        Nan::SyntaxError("Object must have a color property");
      }
      num = color.ToLocalChecked().As<v8::Number>();
      // Nan::MaybeLocal<v8::String> str = Nan::To<v8::String>();
      // Nan::Maybe<uint32_t> d = Nan::To<uint32_t>();
      uint_least32_t intColor = num->NumberValue();
      // std::cout << "Color is " << intColor << std::endl;

      stack[i].color = intColor;



    } else {
      Nan::TypeError("Must pass in an array of objects");
    }

  }
}






NAN_MODULE_INIT(Init) {
  // Nan::Set(target, Nan::New("arraytest").ToLocalChecked(),
  //   Nan::GetFunction(Nan::New<FunctionTemplate>(Arraytest)).ToLocalChecked());
  // Nan::Set(target, Nan::New("objecttest").ToLocalChecked(),
  //   Nan::GetFunction(Nan::New<FunctionTemplate>(Objecttest)).ToLocalChecked());
  // NAN_EXPORT(target, ReadU8);
  NAN_EXPORT(target, UseFilename);
  NAN_EXPORT(target, UseBuffer);
}


NODE_MODULE(myaddon, Init)





