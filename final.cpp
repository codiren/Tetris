#include <windows.h>
#include <commctrl.h>
#include <mshtmcid.h>
#include <winuser.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
PAINTSTRUCT ps;
int kvadrat_dydis = 15;
HBRUSH fonas = CreateSolidBrush(RGB(200, 200, 200));
unsigned long long kadras = 0;
bool pralaimejai = false;
bool eik_zemyn = false;
int score = 0;
HWND hwnd;
class blokas {
	protected:
	public:
	HBRUSH spalva;
	std::vector<std::vector<bool>> kubeliai;
	blokas(auto rgb, auto data) {
		spalva = CreateSolidBrush(rgb);
		kubeliai = data;
	}
	blokas() {}
};
class Iblokas: public blokas {
	public:
	Iblokas() {
		spalva = CreateSolidBrush(RGB(0, 100, 100));
		kubeliai = std::vector<std::vector<bool>> {{1,1,1,1}};
	}
};
class zaidimas {
		std::vector<blokas> detales;
	public:
		std::vector<std::vector<HBRUSH>> drawgridas;
		blokas krentantis_blokas;
		int blok_x,blok_y;
		void game_over() {
			pralaimejai = true;
			MessageBox(NULL, ("Destytojau, jus pralaimejote, jusu taskai: "+std::to_string(score)).c_str(),"Zaidimas baigtas",MB_ICONEXCLAMATION|MB_OK);
			exit(0);
		}
		void operator++(int) {
			static auto neoverlap = [this]()->bool {
				for(int x = 0; x<krentantis_blokas.kubeliai.size(); x++) {
					for(int y = 0; y<krentantis_blokas.kubeliai[0].size(); y++) {
						if(drawgridas[x+blok_x][y+blok_y+1]!=fonas&&krentantis_blokas.kubeliai[x][y])return false;
					}
				}
				return true;
			};
			if(blok_y+krentantis_blokas.kubeliai[0].size()<20&&neoverlap()) {
				return;
			}
			for(int x = 0; x<krentantis_blokas.kubeliai.size(); x++) {
				for(int y = 0; y<krentantis_blokas.kubeliai[0].size(); y++) {
					if(krentantis_blokas.kubeliai[x][y])drawgridas[x+blok_x][y+blok_y] = krentantis_blokas.spalva;
				}
			}

			blok_y = 0;
			krentantis_blokas = detales[rand() % detales.size()];
			blok_x = 5-(krentantis_blokas.kubeliai.size()/2);
			eik_zemyn = false;
			if(!neoverlap())game_over();
		}
		zaidimas() {
			drawgridas = std::vector<std::vector<HBRUSH>>(10, std::vector<HBRUSH>(20,fonas));
			detales.push_back(Iblokas());
			detales.push_back(blokas(RGB(0, 0, 255),std::vector<std::vector<bool>> {{1,0,0},{1,1,1}}));
			detales.push_back(blokas(RGB(255, 165, 0),std::vector<std::vector<bool>> {{0,0,1},{1,1,1}}));
			detales.push_back(blokas(RGB(255, 255, 0),std::vector<std::vector<bool>> {{1,1},{1,1}}));
			detales.push_back(blokas(RGB(0, 255, 0),std::vector<std::vector<bool>> {{0,1,1},{1,1,0}}));
			detales.push_back(blokas(RGB(128, 0, 128),std::vector<std::vector<bool>> {{0,1,0},{1,1,1}}));
			detales.push_back(blokas(RGB(255, 0, 0),std::vector<std::vector<bool>> {{1,1,0},{0,1,1}}));
			krentantis_blokas = detales[rand() % detales.size()];
			blok_x = 5-(krentantis_blokas.kubeliai.size()/2);
		}
		void tikrink_linijas(){
			std::set<int> istrinta;
			for(int y = 0;y<drawgridas[0].size();y++){
				for(int x = 0;x<drawgridas.size();x++){
					if(drawgridas[x][y]==fonas)break;
					if(x==drawgridas.size()-1){
						for(int xx = 0;xx<drawgridas.size();xx++){
							drawgridas[xx][y] = fonas;
						}
						score += 10;
						istrinta.insert(y);
					}
				}
			}
			for(auto i:istrinta){
				for(int y = i;y>0;y--){
					for(int x = 0;x<drawgridas.size();x++){
						drawgridas[x][y] = drawgridas[x][y-1];
					}
				}
			}
		}
};
zaidimas tetris;
std::map<int, void(*)()> zinutes;
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	if(zinutes.find(Message)!=zinutes.end())zinutes[Message]();
	switch(Message) {
		case WM_PAINT: {
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rect;
			GetClientRect(hwnd, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			int pradx = (width/2)-((tetris.drawgridas.size()/2)*kvadrat_dydis);
			int prady = (height/2)-((tetris.drawgridas[0].size()/2)*kvadrat_dydis);
			SelectObject(hdc, fonas);
			Rectangle(hdc,pradx,prady,(width/2)+((tetris.drawgridas.size()/2)*kvadrat_dydis),(height/2)+((tetris.drawgridas[0].size()/2)*kvadrat_dydis));
			SelectObject(hdc, tetris.krentantis_blokas.spalva);
			for(int x = 0; x<tetris.krentantis_blokas.kubeliai.size(); x++) {
				for(int y = 0; y<tetris.krentantis_blokas.kubeliai[x].size(); y++) {
					if(tetris.krentantis_blokas.kubeliai[x][y])Rectangle(hdc, pradx+((x+tetris.blok_x)*kvadrat_dydis), prady+((y+tetris.blok_y)*kvadrat_dydis),
						        pradx+((x+tetris.blok_x+1)*kvadrat_dydis), prady+((y+tetris.blok_y+1)*kvadrat_dydis));
				}
			}
			for(int x = 0; x<tetris.drawgridas.size(); x++) {
				for(int y = 0; y<tetris.drawgridas[0].size(); y++) {
					if(tetris.drawgridas[x][y] != fonas) {
						SelectObject(hdc, tetris.drawgridas[x][y]);
						Rectangle(hdc, pradx+((x)*kvadrat_dydis), prady+((y)*kvadrat_dydis),
						          pradx+((x+1)*kvadrat_dydis), prady+((y+1)*kvadrat_dydis));
					}
				}
			}
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_KEYDOWN:
			if(pralaimejai)break;
			switch (wParam) {
				case VK_DOWN:
					eik_zemyn = true;
					break;
					if(eik_zemyn)break;
				case VK_LEFT:
					if(tetris.blok_x<1)break;
					for(int x = 0; x<tetris.krentantis_blokas.kubeliai.size(); x++) {
						for(int y = 0; y<tetris.krentantis_blokas.kubeliai[x].size(); y++) {
							if(tetris.drawgridas[x+tetris.blok_x-1][y+tetris.blok_y]!=fonas)goto baikk;
						}
					}
					tetris.blok_x--;
					baikk:
					break;
				case VK_RIGHT: {
					if(tetris.krentantis_blokas.kubeliai.size()+tetris.blok_x>9)break;
					for(int x = 0; x<tetris.krentantis_blokas.kubeliai.size(); x++) {
						for(int y = 0; y<tetris.krentantis_blokas.kubeliai[x].size(); y++) {
							if(tetris.drawgridas[x+tetris.blok_x+1][y+tetris.blok_y]!=fonas)goto baik;
						}
					}
					tetris.blok_x++;
					baik:
					break;
				}
				case VK_UP:
					std::vector<std::vector<bool>> kopija = std::vector<std::vector<bool>>(tetris.krentantis_blokas.kubeliai[0].size(), std::vector<bool>(tetris.krentantis_blokas.kubeliai.size()));
					for(int x = 0; x<tetris.krentantis_blokas.kubeliai[0].size(); x++) {
						for(int y = 0; y<tetris.krentantis_blokas.kubeliai.size(); y++) {
							if(x+tetris.blok_x>9)goto baikup;
							kopija[x][y] = tetris.krentantis_blokas.kubeliai[tetris.krentantis_blokas.kubeliai.size()-y-1][x];
							if(kopija[x][y]&&tetris.drawgridas[x+tetris.blok_x][y+tetris.blok_y]!=fonas)goto baikup;
						}
					}
					tetris.krentantis_blokas.kubeliai = kopija;
					baikup:
					break;
			}
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS wc = {0};
	MSG msg;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "WindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	if(!RegisterClass(&wc)) MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
	hwnd = CreateWindowEx(0, "WindowClass", "Tetris", WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,640,480, 0, 0, hInstance, 0);
	if(hwnd == NULL)MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	zinutes[WM_SIZE] = [](){InvalidateRect(hwnd, NULL, TRUE);};
	zinutes[WM_DESTROY] = [](){exit(0);};
	while(1){Sleep(35);
		MSG Message;
        while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        kadras++;
		if(kadras%7 == 0||eik_zemyn) {
			tetris++;
			tetris.blok_y += 1;
		}
		tetris.tikrink_linijas();
		InvalidateRect(hwnd, NULL, TRUE);
		if(pralaimejai)break;
	}
	return msg.wParam;
}

