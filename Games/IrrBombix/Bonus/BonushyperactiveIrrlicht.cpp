/*
 * Copyright 2011
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "BonushyperactiveIrrlicht.h"
#include "BonusXMLitemIrrlicht.h"
#include "BonusIrrlicht.h"
#include <iostream>
#include "../PlayerIrrlicht.h"
#include "../GameIrrlicht.h"
#include "../GameTimerIrrlicht.h"
#include "../Cell.h"

BonushyperactiveIrrlicht::BonushyperactiveIrrlicht(){
	// TODO Auto-generated constructor stub
	//this->setView(this);

	BonusXMLItemIrrlicht* tmp = new BonusXMLItemIrrlicht(this,
			"./media/Items/Bonushyperactive.xml");
	this->setView(tmp);
	tmp->setModel(this);
	done = 0;
	duration=10000.0;
	//std::cout << this->getI() << ", "<< this->getJ() <<" in BonushyperactiveIrrlicht\n";
}

BonushyperactiveIrrlicht::~BonushyperactiveIrrlicht() {
	// TODO Auto-generated destructor stub
}

void BonushyperactiveIrrlicht::apply(PlayerIrrlicht* pl) {
	if (done == 0) {
		done = 1;
		slowspeed = pl->getSpeed() * -2;
		pl->setSpeed(pl->getSpeed() - slowspeed);
		starttime = GameIrrlicht::m_gametimerIrrlicht->getTime();
		pl->m_hyperactive=pl->m_hyperactive+1;
	} else if (done == 1) {
		for (int i=0;i<GameIrrlicht::players.size();i++){
						// Collect Bonus
						 float dx = GameIrrlicht::players[i]->getX()-pl->getX();
						 float dy = GameIrrlicht::players[i]->getY()-pl->getY();

						 if (dx*dx +dy*dy < 0.25*Cell::SIZE*Cell::SIZE && GameIrrlicht::players[i]->m_hyperactive==0){
								 // infect
							 BonushyperactiveIrrlicht* tmp =new BonushyperactiveIrrlicht;
							 GameIrrlicht::players[i]->getBonustaken()->push_back(tmp);
							 tmp->collected=true;
							 }
						 }

		if (GameIrrlicht::m_gametimerIrrlicht->getTime() > +starttime
				+ duration) {
			done == 2;
			pl->setSpeed(pl->getSpeed() + slowspeed);
			// dont update anymore
			int pos =pl->getBonustaken()->linear_search(this);
			if (pos >=0){
				pl->getBonustaken()->erase(pos);
			}
			pl->m_hyperactive=pl->m_hyperactive-1;
		}

	}

}
