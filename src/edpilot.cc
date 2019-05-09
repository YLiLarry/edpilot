#include <edpilot.h>
#include <iostream>
#include <string>

#if APPLE
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif

using namespace std;
using namespace edpilot;
using namespace ggframe;

#if APPLE
using namespace boost;
#endif

void SpeedObserver::trainOnline(vector<FrameFeature> const& features, vector<unsigned> const& speed)
{

}

void SpeedObserver::labelTrainingDataInteractive()
{

	filesystem::path data_dir;
	/* define where data is located */
#if WIN32
	data_dir = "C:\\Users\\Yu\\Desktop\\data";
#endif
#if APPLE
	data_dir = "/Users/yuli/Downloads/data";
#endif
	std::cerr << "checking " << data_dir << std::endl;
	assert(filesystem::is_directory(data_dir));
	Frame pattern;
	filesystem::path template_file = data_dir;
	template_file /= "template.bmp";
	if (filesystem::exists(template_file)) {
		pattern.load(template_file);
	}
	for (auto& file : filesystem::directory_iterator(data_dir))
	{
		string fp = file.path().string();
		if (fp.find(".bmp") != fp.npos && file.path().filename() != "template.bmp") {
			std::cerr << "found img " << fp << std::endl;
			ggframe::Frame frame(fp);
			frame.setGridSize(10);
			InputEvent event;
			Rec mouse_selection;
			if (pattern.empty()) {
				std::cerr << "where is the interested region?" << std::endl;
				Frame tmp = frame;
				do {
					tmp.display();
					event = tmp.waitForInput();
					mouse_selection = frame.bestGridRecCenteredAt(Pos::rc(event.mouse.row(), event.mouse.col()), ggframe::Size::hw(180, 100));
					tmp.drawRec(mouse_selection);
				} while (event.source == InputSource::Mouse && pattern.empty());
				pattern = frame;
				pattern.crop(mouse_selection);
				cerr << "where should I save the pattern?" << endl;
				filesystem::path template_dir = data_dir;
				template_dir /= "template.bmp";
				pattern.save(template_dir);
			} else {
				cerr << "trying to match pattern" << endl;
				Rec matched_rec = frame.recMatchedTemplate(pattern);
				// frame.drawRec(matched_rec);
				// frame.display();
				// cerr << "showing matched rectangle" << endl;
				InputEvent e{};
				while(e.source != Keyboard) {
					e = frame.waitForInput();
				}
			}
		}
	}
}

FrameFeature::FrameFeature(ggframe::Frame const& frame)
{
}

