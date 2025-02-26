// memmy (c) mjt, 25.2.2014
 
#pragma once
#include "Program.h"
 
const int X=4, Y=4;
class Game_Memmu : public Program
{
    Model block[Y][X];
    scene::ISceneNode *selectedSceneNode[2];
    ITexture *bg;
 
    int tries; 
    float _ang;
    int anim;
    int selected;
    int delay;
 
public:
 
    void createScene()
    {
        device->setWindowCaption(L"Memmu");
 
        srand(device->getTimer()->getRealTime());
 
        delay=0;
        _ang=0;
        tries=0;
        const stringc DIR="memmu/";
 
        stringc imgName[]={"1.jpg","2.jpg","3.jpg","4.jpg","5.jpg","6.jpg","7.jpg","8.jpg",
                           "1.jpg","2.jpg","3.jpg","4.jpg","5.jpg","6.jpg","7.jpg","8.jpg",};
 
        bg = driver->getTexture(DATA_DIR + DIR + "bg.jpg");
 
        for(int y=0;y<Y;y++)
        {
            for(int x=0;x<X; x++)
            {
                block[y][x].load(DIR + "memmu_block.3ds");
                block[y][x].getNode()->setMaterialFlag(video::EMF_LIGHTING, false);
                block[y][x].getNode()->setScale(vector3df(0.3f, 0.3f, 0.3f));
                block[y][x].getNode()->setPosition(vector3df(x-1.5f, y-1.5f, 0));
                block[y][x].getNode()->setRotation(vector3df(90, 0, 0));
                //block[y][x].getNode()->setMaterialTexture(0, driver->getTexture(DATA_DIR + "demoback.jpg"));
                block[y][x].getNode()->getMaterial(0).setTexture(0, driver->getTexture(DATA_DIR + DIR + "blk.jpg"));
 
                while(1)
                {
                    int rnd=rand()%16;
                    if(imgName[rnd]=="") continue;
                    block[y][x].getNode()->getMaterial(1).setTexture(0, driver->getTexture(DATA_DIR + DIR + imgName[rnd]));
                    block[y][x].getNode()->setName(imgName[rnd]);
                    block[y][x].getNode()->setID(1);
                    imgName[rnd]="";
                    break;
                }
            }
        }
        IMetaTriangleSelector *meta = createTriangleSelectors();
        meta->drop();
       
        IGUIStaticText *text = guienv->addStaticText(L"Memmu (c) mjt, 2014", rect<s32>(1,1,500,60), false, false, 0, 0 );
        text->setOverrideColor(SColor(255, 100,255,50));
 
        camera = sceneManager->addCameraSceneNode();
        camera->setPosition(vector3df(0,0,-3));
        camera->setNearValue(0.1f);
        camera->setFarValue(1000);
        camera->setTarget(vector3df(0,0,0));
 
        anim=0;
        selected=0;
    }
 
    void clearScene()
    {
        sceneManager->clear();
        guienv->clear();
    }
 
    void render(f32 frameDeltaTime)
    {
        if(input.IsKeyDown(irr::KEY_ESCAPE))
            running=false;  //device->closeDevice();
 
        _ang+=frameDeltaTime;
 
        SColor colors[4];
        for(int q=0;q<4;q++)
            colors[q]=SColor(255, 255,255, (int)(sinf(_ang)*128)+128);
 
 
        driver->enableMaterial2D(true);
        driver->getMaterial2D().Lighting = false;
        driver->getMaterial2D().ZWriteEnable = false;
        driver->getMaterial2D().ZBuffer = false;
        driver->getMaterial2D().BackfaceCulling = false;
        driver->draw2DImage(bg, rect<s32>(0,0,screenWidth, screenHeight),rect<s32>(0,0,bg->getSize().Width, bg->getSize().Width),
                            0,colors);
        driver->enableMaterial2D(false);
 
        sceneManager->drawAll();
 
 
        // jos klikataan eikä animaatiota vielä päällä
        if(input.MouseState.Buttons[0] && anim==0)
        {
            ISceneCollisionManager *collMananger = sceneManager->getSceneCollisionManager();
            triangle3df triangle;
            vector3df point;
            position2di pos=position2di(input.MouseState.Position.X, input.MouseState.Position.Y);
            line3df ray = collMananger->getRayFromScreenCoordinates(pos);
            ISceneNode *newnode = collMananger->getSceneNodeAndCollisionPointFromRay(ray, point, triangle);
            if(newnode && newnode->getID()==1) // jos valittu obu on blokki
            {
                if(selected>0 || newnode!=selectedSceneNode[selected-1]) // jos se ei ole sama joka on jo valittu
                {
                        selectedSceneNode[selected] = newnode;
                        selectedSceneNode[selected]->setScale(vector3df(0.33f, 0.33f, 0.33f));
                        anim=1; // 1==pyöräytä
                        selected++;
                }
            }
        }
 
        // pyöräytä valittu obu
        if(anim==1)
        {
            vector3df r=selectedSceneNode[selected-1]->getRotation();
            if(r.X<270)
            {
                r.X+=5;
                r.Z+=10;
                selectedSceneNode[selected-1]->setRotation(r);
            }
            else
                if(selected==2) // kummatkin obut pyöräytetty oikeinpäin
                {
                    if(stringc(selectedSceneNode[0]->getName()) == stringc(selectedSceneNode[1]->getName()))
                    {
                        anim=2;
                    }
                    else
                    {
                        tries++;
                        anim=-1; // palauta obut 'väärinpäin'
                        delay=0;
                    }
                }
                else anim=0; // että voidaan valita toinen obu
        }
 
        // skaalaus pieneks
        if(anim==2)
        {
            float S=selectedSceneNode[0]->getScale().X-0.01f;
            selectedSceneNode[0]->setScale(vector3df(S,S,S));
            selectedSceneNode[1]->setScale(vector3df(S,S,S));
            if(S<=0)
            {
                selectedSceneNode[0]->setScale(vector3df(0,0,0));
                selectedSceneNode[1]->setScale(vector3df(0,0,0));
                anim=0;
                selectedSceneNode[0]=0;
                selectedSceneNode[1]=0;
                selected=0;
            }
        }
 
        // skip delay
        if(anim==-1 && input.MouseState.Buttons[0])
            delay=50;
 
        // palauta obut 'väärinpäin'
        if(anim==-1 && delay++ >= 50)
        {
            bool stillRotate=false;
            for(int y=0;y<Y; y++)
                for(int x=0;x<X;x++)
                    {
                        if(block[y][x].getNode()->getScale().X==0) continue;
 
                        vector3df r=block[y][x].getNode()->getRotation();
                        if(r.X>90)
                        {
                            stillRotate=true;
                            r.X-=10;
                            r.Z-=20;
                            block[y][x].getNode()->setRotation(r);
                            block[y][x].getNode()->setScale(vector3df(0.3f, 0.3f, 0.3f));
                        }
                    }
           
            if(stillRotate==false)
            {
                anim=0;
                selectedSceneNode[0]=0;
                selectedSceneNode[1]=0;
                selected=0;
            }
        }
 
        bool objleft=false;
        for(int y=0;y<Y; y++)
            for(int x=0;x<X;x++)
                if(block[y][x].getNode()->getScale().X!=0)
                {
                    y=Y;
                    objleft=true;
                    break;
                }
 
        guienv->drawAll();
       
        stringc triestxt="Tries: ";
        triestxt+=tries;
        font->draw(triestxt, rect<s32>(1,35, 50,300), SColor(255,255,0,0));
 
        if(!objleft)
        {
            int x=(int)(sin(_ang)*60)+60;
            int y=(int)(cos(_ang)*100)+80;
            font->draw("Game over!", rect<s32>(x,y, 50,300), SColor(255,255,255,255));
        }
 
    }
};
