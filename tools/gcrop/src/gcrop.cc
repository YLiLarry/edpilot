#include <iostream>
#include <string>
#include <ggframe.h>

#if APPLE
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif

#if APPLE
using namespace boost;
#endif

using namespace std;

int main(int argc, char** argv)
{
    filesystem::path data_dir;
    /* define where data is located */
#if WIN32
    data_dir = "C:\\Users\\Yu\\Desktop\\data";
#endif
#if APPLE
    data_dir = "/Users/yuli/Downloads/data";
#endif
    filesystem::path template_dir = data_dir / "templates";
    filesystem::path labeled_dir = data_dir / "labeled";

    filesystem::path image_path;
    if (argc < 2) {
        cerr << "where is the image to crop?" << endl;
        cin >> image_path;
    } else {
        image_path = argv[1];
    }
    assert(filesystem::exists(image_path));
    ggframe::Frame frame(image_path);
    std::cerr << "found img " << image_path << std::endl;
    frame.setGridSize(10);
    ggframe::InputEvent event;
    ggframe::Rec best_rec;
    std::cerr << "where is the interested region?" << std::endl;
    while(true) {
        frame.display();
        event = frame.waitForInput();
        if (event.source == ggframe::Mouse) {
            best_rec = frame.bestGridRecCenteredAt(event.mouse, ggframe::Size::hw(150,100));
            frame.load(image_path);
            frame.drawRec(best_rec);
        } else if (event.source == ggframe::Keyboard) {
            filesystem::path cropped_img_path = image_path.parent_path() / ("crop_" + image_path.filename().string());
            frame.crop(best_rec);
            frame.display();
            frame.save(cropped_img_path);
            cerr << "output to" << cropped_img_path << endl;
            break;
        } 
    }
}

