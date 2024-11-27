#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -2*BUBBLE_DEFAULT_VX, 0, COLOR(255,128,0)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, 2*BUBBLE_DEFAULT_VX, 0, COLOR(255,128,0)));
    bubbles.push_back(Bubble(3*WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, 2*BUBBLE_DEFAULT_VX, 0, COLOR(255,128,0)));
    return bubbles;
}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    int scr=0, hlth=3, tm=0,stm=0; // scr-Score, hlth-Health, tm=time, stm= time in seconds
    unsigned int k=0; //to store no. of bubbles currently not intersecting with shooter
    bool sht_int=false, gm_or=false; //sht_int stores shooter bubble intersection and gm_or stores gameover





    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    string score("Score: 00");
    Text scoreobt(WINDOW_X-LEFT_MARGIN, BOTTOM_MARGIN, score);



    string health("Health: 3/3");
    Text crnthealth(WINDOW_X-LEFT_MARGIN,TOP_MARGIN , health);

    Text gameover(WINDOW_X/2.0,WINDOW_Y/2.0,""); //to display "Game Over" message
    gameover.setColor(COLOR(255,0,0));

    Text congrats(WINDOW_X/2.0,WINDOW_Y/2.0,""); //to display "Congratulations!!" message
    congrats.setColor(COLOR(0,0,255));

    string time("Time: 00/50");
    Text timer(LEFT_MARGIN, TOP_MARGIN, time);



    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        //checking bullet-bubble collisions
         for(unsigned int i=0; i<bullets.size(); i++)
         {
         if(gm_or) //to stop bubble dynamics after the game gets over
         break;

           for (unsigned int j=0; j < bubbles.size(); j++)
             { if((bullets[i].get_center_y()-bullets[i].get_height()/2.0)<=(bubbles[j].get_center_y()+bubbles[j].get_radius())
                   &&
                   (bullets[i].get_center_y()-bullets[i].get_height()/2.0)>=(bubbles[j].get_center_y()-bubbles[j].get_radius())
                   &&
                   bullets[i].get_center_x()<=(bubbles[j].get_center_x()+bubbles[j].get_radius())
                   &&
                   bullets[i].get_center_x()>=(bubbles[j].get_center_x()-bubbles[j].get_radius())
                 )

                 {
                   if(!(bubbles[j].get_radius()/2>=(BUBBLE_RADIUS_THRESHOLD/2-1)
                        &&
                        bubbles[j].get_radius()/2<=(BUBBLE_RADIUS_THRESHOLD/2+1)
                       )


                     )

                     {// spliting larger bubbles into smaller
                     bubbles.push_back(Bubble(bubbles[j].get_center_x(),bubbles[j].get_center_y(),bubbles[j].get_radius()/2, 2*BUBBLE_DEFAULT_VX, 0, COLOR(255,128,0)));
                     bubbles.push_back(Bubble(bubbles[j].get_center_x(), bubbles[j].get_center_y(), bubbles[j].get_radius()/2, -2*BUBBLE_DEFAULT_VX, 0, COLOR(255,128,0)));
                     bubbles.erase(bubbles.begin()+j);
                     bullets.erase(bullets.begin()+i);

                     }
                   else
                       {//for smallest possible bubbles to disappear
                       bubbles.erase(bubbles.begin()+j);
                       bullets.erase(bullets.begin()+i);


                       }
                    //updating score
                   scr++;

                   score[score.length() - 1] = '0'+scr%10;
                   score[score.length() - 2] = '0'+scr/10;

                   scoreobt.setMessage(score);


                 }



             }
         }
        //checking bubble-shooter collisions
         for (unsigned int j=0; j < bubbles.size(); j++)
           {
             if((abs(bubbles[j].get_center_x()-shooter.get_body_center_x())<(shooter.get_body_width()/2.0+bubbles[j].get_radius())
               &&
                 abs(bubbles[j].get_center_y()-shooter.get_body_center_y())<(shooter.get_body_height()/2.0+bubbles[j].get_radius())
                )

               ||
               ((pow(shooter.get_head_center_x()-bubbles[j].get_center_x(),2)+pow(bubbles[j].get_center_y()-shooter.get_head_center_y(),2))
                 <
                 pow((bubbles[j].get_radius()+shooter.get_head_radius()),2))


               )
               {
               if(sht_int) //to not decrease health by just one collision (a collision covers many STEP_TIME )
               break;


               if(hlth==0) //to not decrease health beyond 0
               break;

               sht_int=true;
               //changing shooter color during colllision
               shooter.setcolorofshooter(COLOR(153,255,204));

               //updating health
               hlth--;
               health[health.length() - 3] = '0'+hlth;

               crnthealth.setMessage(health);

              }
            else
            k++; //updating k

            }

        if(k==bubbles.size())
        {//restoring original color of shooter after collision
            shooter.setcolorofshooter(COLOR(0,255,0));

            sht_int=false;


        }
        else
        k=0; //re-assigning k to repeat the loop

        if(
           hlth==0
           ||
           (time[time.length() - 4]=='0'
            &&
            time[time.length() - 5]=='5')


           )
           {
            gameover.setMessage("Game Over");     //displaying "Game Over"


            //to freeze everything after the game gets over
            for (unsigned int j=0; j < bubbles.size(); j++)
            {
            bubbles[j].setvxvy(0,0);
            }


            STEP_TIME=0;
            gm_or=true; //to stop bubble dynamics after the game gets over
            continue; //to freeze time



           }

        if(scr==21)
          {
             congrats.setMessage("Congratulations!!");  //displaying "Congratulations!!"
             STEP_TIME=0; //to freeze shooter
             continue; //to freeze time


          }

        wait(STEP_TIME);

        //updating time
        tm++;
        stm=tm/24;

        time[time.length() - 4] = '0'+stm%10;
        time[time.length() - 5] = '0'+stm/10;

        timer.setMessage(time);



    }
}
