// The code is distributed under the GNU GPL v3
// Part of the OSRPG project: http://osrpgcreation.sf.net
// Do you want to create the best RPG? Join us!
// Copyright (C) 2006 Michael Kapelko (aka kornerr)

#ifndef OSRPG_EDITOR_GUI_SAVE_AS_H
#define OSRPG_EDITOR_GUI_SAVE_AS_H

#include "../map/scene.h"

class SaveAs {
	public:
		SaveAs (Registry *reg, MapScene *ms);
		~SaveAs ();
		void OnCancel ();
		bool OnEvent (const irr::SEvent &e);
		void OnOk ();
		void Show (bool state = true);

	private:
		Registry *reg;
		MapScene *ms;
		irr::gui::IGUIWindow *wn;
		irr::gui::IGUIButton *ok_btn,
			*cancel_btn;
		irr::gui::IGUIListBox *list;
		irr::gui::IGUIEditBox *edit;
		irr::core::stringc map_dir;
};

#endif // OSRPG_EDITOR_GUI_SAVE_AS_H

