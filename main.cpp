#include "raylib.h"
#include "rcamera.h"
#include<math.h>
#include<cmath>
#include<cstdlib>
#include<ctime>
using namespace std;

//forward declarations
class projectile;
class targetEnemy;


bool offscreen(Vector2 z){
    if (z.x<-10||z.x>820||z.y<-10||z.y>460) 
    return true; 
    return false;
}

float magnitude(Vector2 v){
    return (float)sqrt(pow(v.x,2)+pow(v.y,2));
}

class entity{
    protected:
    Vector2 position;
    int speed;

    entity(){
        position={0,0};
        speed=4;
    }

    virtual void walk()=0;
};


class bow{
    Vector2 direction = { 0, 0 };
    float rotation;
    Texture2D bow_image;

    public:

    bow(){

        direction={0,0};
    }

    void init(){bow_image=LoadTexture("bow2.png");}

    void walk(Vector2 pos){
        direction={GetMousePosition().x-pos.x,GetMousePosition().y-pos.y};
        rotation=atan2f(direction.y, direction.x) * RAD2DEG +130;
    }

    void disp(Vector2 pos){
        Rectangle source = { 0, 0, (float)bow_image.width, (float)bow_image.height };
        Rectangle dest = { pos.x, pos.y, (float)bow_image.width/2, (float)bow_image.height/2 };
        Vector2 origin = { (float)bow_image.width / 2, (float)bow_image.height / 2 };

        DrawTexturePro(bow_image, source, dest, origin, rotation, WHITE);
    }

    float getrotation(){
        return rotation;
    }

    void operator--(){
        UnloadTexture(bow_image);
    }
};



class player:public entity{
    private:
    Color PlayerColor;
    Texture2D playerimg;
    float size;

    public:

    player():entity(),size(30){
        PlayerColor=BLACK;
    }

    void init(){
        playerimg=LoadTexture("charachter.png");
        this->position={500.0f,250.0f};
    }

    void walk(){
        if(IsKeyDown(KEY_A)) this->position.x -= this->speed;
        if(IsKeyDown(KEY_W)) this->position.y -= this->speed;
        if(IsKeyDown(KEY_S)) this->position.y += this->speed;
        if(IsKeyDown(KEY_D)) this->position.x += this->speed;

        if (this->position.x<0)this->position.x=0;
        if (this->position.y<0)this->position.y=0;

    }

    Vector2 getpos(){return position;}

    void drawball(float r){
        //DrawCircleV(this->position,this->size,this->PlayerColor);
        Rectangle source = { 0, 0, (float)playerimg.width, (float)playerimg.height };
        Rectangle dest = { position.x, position.y, (float)playerimg.width/1.3, (float)playerimg.height/1.3};
        Vector2 origin = { (float)playerimg.width/2.6, (float)playerimg.height/2.6 };
        //origin = {0.0f,0.0f };d
        DrawTexturePro(playerimg,source,dest,origin,r-45,WHITE);
    }
    void operator--(){
        UnloadTexture(playerimg);
    }
    friend class projectile;
    friend class targetEnemy;
    friend bool operator==(player& pl,targetEnemy& tE);

};

class projectile{
    Texture2D arrow_img;
    bool hidden;
    Vector2 position;
    int speed;
    Vector2 direction;
    float rotation;
    public:
        projectile(){
            hidden=true;
            speed=25;
        }

        void init(){
            arrow_img=LoadTexture("projectile.png");
        }
        
        void walk(player pl,bow& bw){
            if (hidden){
                position=pl.position;
            }
            else if (offscreen(position)){
                hidden=true;
                position=pl.position;
            }
            else{
                position.x+=speed*cosf(rotation*DEG2RAD);
                position.y+=speed*sinf(rotation*DEG2RAD);
            }

            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                shoot(pl,bw);
            }
        }
        void shoot(player& pl,bow& bw){
            if (hidden){
                hidden=false;
                position=pl.position;
                rotation=bw.getrotation()-45-89;
            }
        }
        void disp(){
            if(!hidden){
                //DrawCircleV(position,5,BLACK);
                Rectangle source = { 0, 0, (float)arrow_img.width, (float)arrow_img.height };
                Rectangle dest = { position.x, position.y, (float)arrow_img.width/1.3, (float)arrow_img.height/1.3};
                Vector2 origin = { (float)arrow_img.width/2.6, (float)arrow_img.height/2.6 };
                //origin = {0.0f,0.0f };
                if(IsKeyDown(KEY_I)&&IsKeyDown(KEY_L))
                DrawTexturePro(arrow_img,source,dest,origin,rotation+90,WHITE);
                else DrawTexturePro(arrow_img,source,dest,origin,rotation,WHITE);
            }
        }

        Vector2 getpos(){
            return position;
        }
        bool isactive(){
            return !hidden;
        }

        friend class shootable;//because the projectile needs to shoot the shootable objects

        void operator--(){
            UnloadTexture(arrow_img);
        }
};
 
class shootable{
    public:
    Vector2 direction;
    Vector2 position;
    virtual void disp()=0;
    bool is_shot(projectile& pr){
        return sqrt((pr.position.x-this->position.x)*(pr.position.x-this->position.x) + (pr.position.y-this->position.y)*(pr.position.y-this->position.y))<40 ;
    }
    
};

class targetEnemy:public shootable{
    Vector2 reset_position;

    public:
    targetEnemy(){
        direction={-100,-100};
        position={-100,-100};
        reset_position={-100,-100};
    }

    void init(float a,float b){
        reset_position={a,b};
        position={a,b};
    }

    void point(player& pl){
        Vector2 newdir=pl.position;
        newdir.x=newdir.x-this->position.x;
        newdir.y=newdir.y-this->position.y;
        direction=newdir;
    }
    void walk(player& pl){
        this->point(pl);
        float mag=magnitude(this->direction);
        if (mag!=0){
            position.x+=3*direction.x/mag;
            position.y+=3*direction.y/mag;
        }
    }
    void posReset(){
        position=reset_position;
    }
    void disp(){
        DrawCircleV(position,30,BLACK);
    }
};

class staticEnemy:public shootable{
    void disp(){
        DrawCircleV(position,30,BLACK);
    }
};

bool operator==(player& pl,targetEnemy& tE){
    return sqrt((pl.position.x-tE.position.x)*(pl.position.x-tE.position.x) + (pl.position.y-tE.position.y)*(pl.position.y-tE.position.y))<70 ;
}

class game{
    private:
    bow weapon;
    player myplayer;
    Texture2D backdrop;
    Texture2D back;
    projectile playerArrow;
    targetEnemy mover1;
    targetEnemy mover2;
    targetEnemy mover3;
    int points=0;

    void initialize(){
        const int screenWidth = 800;
        const int screenHeight = 450;
        InitWindow(screenWidth, screenHeight, "TDS OOP LAB");
        SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
        weapon.init();
        myplayer.init();
        playerArrow.init();
        backdrop=LoadTexture("backdrop.png");
        back=LoadTexture("pbd.png");
        mover1.init(-100,-100);
        mover2.init(-100,490);
        mover3.init(900,-100);
    }

    void starting_screen(){
        bool running=true;
        while (!WindowShouldClose()&&running)
        {
            BeginDrawing();
                ClearBackground(LIGHTGRAY);
                DrawTexture(back,0,0,WHITE);
                DrawText("PRESS WASD TO START", 150, 150,40,BLACK);
            EndDrawing();

            if(IsKeyDown(KEY_A)) running=false;
            if(IsKeyDown(KEY_W)) running=false;
            if(IsKeyDown(KEY_S)) running=false;
            if(IsKeyDown(KEY_D)) running=false;
        }

    }

    void gameloop(player& Player,bow& playerbow){
        Color bc={140, 184, 172};
        bool running = true;
            while (!WindowShouldClose() && running)    // Detect window close button or ESC key
        {   
            mover1.walk(Player);
            mover2.walk(Player);
            mover3.walk(Player);

            Player.walk();
            playerbow.walk(Player.getpos());
            playerArrow.walk(Player,playerbow);

            if(mover1.is_shot(playerArrow)){
                mover1.posReset(); points++;
            }
            
            else if(mover2.is_shot(playerArrow)){
                mover2.posReset();points++;
            }

            else if (mover3.is_shot(playerArrow)){
                mover3.posReset();points++;
            }
            


            BeginDrawing();
                ClearBackground(bc);
                DrawTexture(backdrop,0,0,WHITE);
                playerArrow.disp();

                if (points<3) DrawText("SHOOT THE ENEMIES\nuse WASD and the mouse to shoot", 150, 150,30,BLACK);

                DrawText(TextFormat("points :%d",points), 30, 30,30,BLACK);
                playerbow.disp(Player.getpos());
                Player.drawball(playerbow.getrotation());
                mover1.disp();
                mover2.disp();
                mover3.disp();
                
            EndDrawing();

            if(Player==mover1) running = false;
            if(Player==mover2)running = false;
            if(Player==mover3)running = false;
            
        }
    }

    void ending_screen(){
        bool running=true;
        while (!WindowShouldClose()&&running)
        {
            BeginDrawing();
                ClearBackground(LIGHTGRAY);
                DrawTexture(back,0,0,WHITE);
                DrawText(TextFormat("You Lost! your final score is %d",points), 80, 150,40,BLACK);
            EndDrawing();

            if(IsKeyDown(KEY_Q)) running=false;
        }
    }

    public:
    game(){}

    void run(){
        this->initialize();
        this->starting_screen();
        this->gameloop(myplayer,weapon);
        this->ending_screen();
    }

    ~game(){
        --weapon;
        --myplayer;
        --playerArrow;
        UnloadTexture(backdrop);
        UnloadTexture(back);
        CloseWindow();
    }
};


int main(void)
{
    game* mygame=new game ;
    mygame->run();
    delete mygame;
    return 0;
}