#include <vector>
#include <memory>
#include <ggframe.h>

namespace edpilot
{

using namespace std;

class FrameFeature
{
public:
	unique_ptr<uint8_t> vector;
	FrameFeature(ggframe::Frame const&);
	FrameFeature(FrameFeature const&) = delete;
	FrameFeature const& operator=(FrameFeature const&) = delete;
};

struct Speed
{
	float norm;
	enum Unit {
		M,
		KM,
		C
	};
	Unit unit;
};

struct ShipMovementState
{
	unsigned vec_x;
	unsigned vec_y;
	unsigned vec_z;
	unsigned vec_c;
};

class SpeedObserver
{
	ShipMovementState m_ship_movement;
	Speed labelFrame(FrameFeature const& feature);
public:
	void labelTrainingDataInteractive();
	void trainOnline(vector<FrameFeature> const& features, vector<unsigned> const& speed);
	unsigned test(vector<FrameFeature> const& features, vector<unsigned> const& speed);
	void updateState(FrameFeature feature);
};

class Pilots
{
	ShipMovementState m_ship_movement;
public:
	void train();
	void test();

	void updateState(FrameFeature feature);
};

}
