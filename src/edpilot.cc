#include "edpilot.h"
#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
using namespace edpilot;
using namespace ggframe;

void SpeedObserver::trainOnline(vector<FrameFeature> const& features, vector<unsigned> const& speed)
{

}

void SpeedObserver::labelTrainingDataInteractive()
{

	filesystem::path data_dir;
	do {
		std::cout << "Where can I find the images?" << std::endl;
		string input;
		std::getline(std::cin, input);
		if (input.length() == 0) {
			input = "C:\\Users\\Yu\\Desktop\\data";
		} else if (input.length() && input[0] == '"')
		{
			input = input.substr(1, input.length() - 2);
		}
		data_dir = input;
		std::cout << "checking " << data_dir << std::endl;
	} while (!filesystem::is_directory(data_dir));
	Frame pattern;
	for (auto& file : filesystem::directory_iterator(data_dir))
	{
		string fp = file.path().string();
		if (fp.find(".bmp") != fp.npos) {
			std::cout << "found img " << fp << std::endl;
			ggframe::Frame frame(fp);
			if (! pattern.empty()) {
				cout << "trying to match pattern" << endl;
				Rec matched_rec = frame.findPattern(pattern);
				frame.drawRec(matched_rec);
				frame.display();
			}
			std::cout << "where is the interested region?" << std::endl;
			frame.setGridSize(10);
			frame.display();
			InputEvent event = frame.waitForInput();
			Rec mouse_selection;
			while (event.source == InputSource::Mouse)
			{
				frame.load(fp);
				mouse_selection = frame.bestGridRecCenteredAt(event.mouse.r, event.mouse.c, 100, 180);
				frame.drawRec(mouse_selection);
				frame.display();
				event = frame.waitForInput();
			}
			if (pattern.empty()) {
				pattern = frame.cutRec(mouse_selection);
			}
			if (event.type == InputEventType::WindowClose) {
				return;
			}
		}
	}
}

FrameFeature::FrameFeature(ggframe::Frame const& frame)
{
}

