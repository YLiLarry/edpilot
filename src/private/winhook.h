#include<functional>
#include<windows.h>

namespace GGInput {
	using namespace std;
	class WinHook
	{
	private:
		function<void> m_callback;
	public:
		void setCallBack(function<void> callback);
		LRESULT winHookProc(int code, WPARAM wParam, LPARAM lParam);
	};
}

