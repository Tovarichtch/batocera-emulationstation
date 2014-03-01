#include "GuiMenu.h"
#include "GuiSettingsMenu.h"
#include "GuiScraperStart.h"
#include "../Window.h"
#include "../Sound.h"
#include "../Log.h"
#include "GuiMsgBoxYesNo.h"

GuiMenu::GuiMenu(Window* window) : GuiComponent(window), mMenu(window, "Main Menu")
{
	struct MenuEntry
	{
		const char* name;
		unsigned int color;
		bool add_arrow;
		std::function<void()> func;
	};

	MenuEntry entries[] = {
		{ "GENERAL SETTINGS", 0x777777FF, true, 
			[this] { mWindow->pushGui(new GuiSettingsMenu(mWindow)); }
		},
		{ "SCRAPE NOW", 0x777777FF, true, 
			[this] { mWindow->pushGui(new GuiScraperStart(mWindow)); }
		},
		{ "RESTART SYSTEM", 0x990000FF, false, 
			[this] {
				mWindow->pushGui(new GuiMsgBoxYesNo(mWindow, "Do you really want to restart the system?",
					[] { 
						if(system("sudo shutdown -r now") != 0)
							LOG(LogWarning) << "Restart terminated with non-zero result!";
					}));
			}
		}, 
		{ "SHUTDOWN SYSTEM", 0x990000FF, false, 
			[this] {
				mWindow->pushGui(new GuiMsgBoxYesNo(mWindow, "Do you really want to shutdown the system?",
					[] { 
						if(system("sudo shutdown -h now") != 0)
							LOG(LogWarning) << "Shutdown terminated with non-zero result!";
					}));
			}
		},
		{ "EXIT EMULATIONSTATION", 0x990000FF, false,
			[] {
				SDL_Event ev;
				ev.type = SDL_QUIT;
				SDL_PushEvent(&ev);
			}
		}
	};

	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());
	mMenu.setPosition((mSize.x() - mMenu.getSize().x()) / 2, (mSize.y() - mMenu.getSize().y()) / 2);

	std::shared_ptr<Font> font = Font::get(FONT_SIZE_LARGE);
	
	// populate the list
	ComponentListRow row;

	for(int i = 0; i < (sizeof(entries) / sizeof(entries[0])); i++)
	{
		row.elements.clear();
		row.addElement(std::make_shared<TextComponent>(mWindow, entries[i].name, font, entries[i].color), true);

		if(entries[i].add_arrow)
			row.addElement(std::make_shared<TextComponent>(mWindow, ">", font, entries[i].color), false);

		std::function<void()>& execFunc = entries[i].func;
		row.input_handler = [execFunc](InputConfig* config, Input input) -> bool
			{
				if(config->isMappedTo("a", input) && input.value != 0)
				{
					execFunc();
					return true;
				}
				return false;
		};

		mMenu.addRow(row);
	}
	
	addChild(&mMenu);
}

bool GuiMenu::input(InputConfig* config, Input input)
{
	if((config->isMappedTo("b", input) || config->isMappedTo("menu", input)) && input.value != 0)
	{
		delete this;
		return true;
	}

	return GuiComponent::input(config, input);
}
