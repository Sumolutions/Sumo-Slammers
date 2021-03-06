#include "Logic.h"
#include "SFML/Audio.hpp"
#include <cmath>
#include <set>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>

Collidable* getActorById(long int id, std::vector<Collidable*>& actors)
{
    for (unsigned int w=0; w<actors.size(); w++) {
        if (actors[w]->getID() == id) {
            return actors[w];
        }
    }
    //throw std::invalid_argument("No such ID in actor vector");
    return NULL;
}

float findCurrentDistance(Wrestler* wrest1, Wrestler* wrest2)
{
    //This function finds the distance between two wrestlers for the current frame
    sf::Vector2f w1_center(wrest1->getPos().x + wrest1->getWidth()/2, wrest1->getPos().y + wrest1->getHeight()/2);
    sf::Vector2f w2_center(wrest2->getPos().x + wrest2->getWidth()/2, wrest2->getPos().y + wrest2->getHeight()/2);
    sf::Vector2f diff = w1_center - w2_center;

    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}

Wrestler* findNearestCollidingWrestler(LocationalMap& loc_map, std::vector<Collidable*>& actors, Wrestler* wrest1)
{
    //This function will find the nearest of all objects colliding with wrest1 for the current frame
    std::set<Wrestler*> collision_set;
    std::vector<long int> tmp;
    std::vector<Wrestler*> sel_wrests;

    for (int i=0; i<loc_map.getRows(); i++) {
        for (int j=0; j<loc_map.getCols(); j++) {

            tmp = loc_map.getCell(i, j).getIDs();

            // if there's wrestlers colliding with the one we set (Need to check if the vector contains wrest1)
            if (tmp.size() >= 2 && std::find(tmp.begin(), tmp.end(), wrest1->getID()) != tmp.end()) {

                //Find the wrestlers in the main list from their ids and choose those that are not the one we set and that are wrestlers
                for (unsigned int idi=0; idi<tmp.size(); idi++){
                    if (tmp[idi] != wrest1->getID()){

                        Wrestler* other = dynamic_cast<Wrestler*>(getActorById(tmp[idi], actors));
                        if (other){
                            sel_wrests.push_back(other);
                        }
                    }
                }

                // create rectangle shapes to detect collisions from where the object is
                sf::RectangleShape
                wrest_box1(sf::Vector2f(wrest1->getWidth(),wrest1->getHeight()));
                wrest_box1.setPosition(wrest1->getPos());
                sf::FloatRect w1_bounds = wrest_box1.getGlobalBounds();

                //Have to compare everything to our wrestler
                for (unsigned int w=0; w<sel_wrests.size(); w++){

                    sf::RectangleShape
                    wrest_box2(sf::Vector2f(sel_wrests[w]->getWidth(),sel_wrests[w]->getHeight()));
                    wrest_box2.setPosition(sel_wrests[w]->getPos());

                    sf::FloatRect w2_bounds = wrest_box2.getGlobalBounds();

                    //Check if the bounding boxes intersect
                    if (w1_bounds.intersects(w2_bounds)) {
                        collision_set.insert(sel_wrests[w]);
                    }
                }
            }
            tmp.clear();
            sel_wrests.clear();
        }
    }
    //Now need to find the nearest of Collising wrestlers
    Wrestler* nearest = NULL;
    float nearest_dist = std::numeric_limits<float>::max();

    std::set<Wrestler*>::iterator it;
    for (it = collision_set.begin(); it != collision_set.end(); ++it){
        float dist = findCurrentDistance(wrest1, *it);
        if (dist < nearest_dist){
            nearest = *it;
            nearest_dist = dist;
        }
    }
    return nearest;
}

std::vector<std::set<long int> > findFutureCollisions(LocationalMap& loc_map,
                                                      std::vector<Collidable*>& actors)
{
    std::vector<std::set<long int> > collision_sets;
    std::vector<long int> tmp;
    std::vector<Collidable*> sel_actors;
    bool in_set = false;

    for (int i=0; i<loc_map.getRows(); i++) {
        for (int j=0; j<loc_map.getCols(); j++) {
            tmp = loc_map.getCell(i, j).getIDs();

            // if there's possibly colliding items
            if (tmp.size() >= 2) {

                //Find the wrestlers in the main list from their ids and put the location in main list in vector parallel to tmp
                for (unsigned int idi=0; idi<tmp.size(); idi++){
                    sel_actors.push_back(getActorById(tmp[idi], actors));
                }

                //Have to compare everything in the cell to each other, but omit same object and alternate permutations
                for (unsigned int w1=0; w1<sel_actors.size(); w1++){
                    for (unsigned int w2=w1+1; w2<sel_actors.size(); w2++){

                        // create rectangle shapes to detect collisions from where the object will be next frame
                        sf::RectangleShape
                        wrest_box1(sf::Vector2f(sel_actors[w1]->getWidth(),sel_actors[w1]->getHeight()));
                        wrest_box1.setPosition(sel_actors[w1]->getMovedPos(loc_map.getLevelWidth(),
                        loc_map.getLevelHeight()));

                        sf::RectangleShape
                        wrest_box2(sf::Vector2f(sel_actors[w2]->getWidth(),sel_actors[w2]->getHeight()));
                        wrest_box2.setPosition(sel_actors[w2]->getMovedPos(loc_map.getLevelWidth(),
                        loc_map.getLevelHeight()));

                        sf::FloatRect w1_bounds = wrest_box1.getGlobalBounds();
                        sf::FloatRect w2_bounds = wrest_box2.getGlobalBounds();

                        //Check if the bounding boxes intersect
                        if (w1_bounds.intersects(w2_bounds)) {

                            //Have to see if the pair is already in a set
                            for (unsigned int cs=0; cs<collision_sets.size(); cs++){
                                if ((collision_sets[cs].find(tmp[w1]) != collision_sets[cs].end()) &&
                                    (collision_sets[cs].find(tmp[w2]) != collision_sets[cs].end())){

                                    in_set = true;
                                    break;
                                }
                            }

                            //insert ids into a new set if not already in a set
                            if (!in_set){

                                //Insert ids for no more collisions this iteration
                                std::set<long int> new_set;
                                new_set.insert(tmp[w1]);
                                new_set.insert(tmp[w2]);

                                collision_sets.push_back(new_set);
                                in_set = false;
                            }
                        }
                    }
                }
            }
            tmp.clear();
            sel_actors.clear();
        }
    }
    return collision_sets;
}

void grabProcedure(Wrestler* w, LocationalMap& loc_map,
std::vector<Collidable*>& actors, sf::Sound *sound)
{
    //How far away the wrestler can grab
    int range = 20;

    int w_height = w->getHeight();
    int w_width = w->getWidth();
    sf::Vector2f w_pos = w->getPos();

    //Make the wrestler bigger so can grab objects that aren't immediately next to it
    w->setHeight(w_height + range*2);
    w->setWidth(w_width + range*2);
    w->setPos(w_pos.x - range, w_pos.y - range);

    //Check the current locations to find intersections
    loc_map.addCurrent(actors);

    Wrestler* other = findNearestCollidingWrestler(loc_map, actors, w);

    loc_map.clearCells();

    //Return wrestler to its initial size
    w->setHeight(w_height);
    w->setWidth(w_width);
    w->setPos(w_pos.x, w_pos.y);

    if (other && other->getName() != "Takeshi"){
        sf::Vector2f old_pos = other->getPos();

        //Move the grabee toward the grabber
        int new_x = old_pos.x;
        int new_y = old_pos.y;

        if (old_pos.x + other->getWidth() < w_pos.x){
            new_x = w_pos.x - other->getWidth() - 1;
        }
        else if (old_pos.x > w_pos.x + w_width){
            new_x = w_pos.x + w_width + 1;
        }
        if (old_pos.y + other->getHeight() < w_pos.y){
            new_y = w_pos.y - other->getHeight() - 1;
        }
        else if (old_pos.y > w_pos.y + w_height){
            new_y = w_pos.y + w_height + 1;
        }

        other->setPos(new_x, new_y);

        //This shouldn't happen very often, but don't allow the grab if repositioning the wrestler causes interpenetration
        loc_map.addCurrent(actors);
        if (findNearestCollidingWrestler(loc_map, actors, other)){
            other->setPos(old_pos.x, old_pos.y);
        }

        //Can't grab with too low stamina or if the other wrestler is dashing
        else if(w->getStamina() > 24 && other->getCurrentState() != Wrestler::DASH){
            sound->play();
            w->useGrab(other, loc_map.getLevelWidth(), loc_map.getLevelHeight());
        }
        loc_map.clearCells();
    }
    //Missed the grab, PENALTY
    else if (w->getStamina() > 24){
        sound->play();
        w->depleteStamina(25);
        w->setCurrentState(Wrestler::NOGRAB);
        w->resetFrozenFrames();
    }
}

void updateDeaths(LocationalMap& loc_map, std::vector<Collidable*> &actors,
Profile& profile, LevelHandler& lev_handler, int &num_bad_guys, sf::Sound
*wilhelm)
{
    std::vector<long int> tmp;
    Collidable* tmp_actor;
    // loop through map and check if an actor is still on the stage
    for (int row=0; row<loc_map.getRows(); row++) {
        for (int col=0; col<loc_map.getCols(); col++) {
            if (loc_map.getCell(row,col).isStandable()) {
                tmp = loc_map.getCell(row, col).getIDs();
                for (unsigned int i=0; i<tmp.size(); i++) {
                    tmp_actor = getActorById(tmp[i], actors);
                    tmp_actor->setOffEdge(false);
                }
            }
        }
    }

    // kill all the actors that are off the stage
    for (unsigned int i=0; i<actors.size(); i++) {
        if (actors[i]->offEdge()) {
            if (actors[i]->isWrestler()) {
                Wrestler* tmp_wrestler = dynamic_cast<Wrestler*>(actors[i]);
                if (tmp_wrestler->isHuman()) {
                    profile.setLives(profile.livesRemaining()-1);
                    tmp_wrestler->reset(actors, lev_handler.getCheckpoint(profile));
                }
                else { // bad guys
                    delete actors[i];
                    actors.erase(actors.begin()+i);
                    num_bad_guys--;
                    wilhelm->play();
                }
            }
            else { // projectiles
                delete actors[i];
                actors.erase(actors.begin()+i);
            }
        }
    }

    for (unsigned int i=0; i<actors.size(); i++) {
        actors[i]->setOffEdge(true);
    }

}

void moveActors(std::vector<Collidable*> &actors, LocationalMap& loc_map,
Profile& profile, LevelHandler& lev_handler, sf::View sf_view, int
&num_bad_guys, sf::Sound *wilhelm)
{
    for (unsigned int i=0; i<actors.size(); i++) {
        actors[i]->move(loc_map.getFriction(), loc_map.getLevelWidth(),
        loc_map.getLevelHeight());

        // if a wrestler stopped moving, update to standing sprite
        if (actors[i]->isWrestler()) {
            Wrestler *w = dynamic_cast<Wrestler*>(actors[i]);
            if (w->getVelocity().x == 0 && w->getVelocity().y == 0) {
                Wrestler::SpriteState state = w->getCurSpriteState();
                if (state == Wrestler::RUN_RIGHT1 || state ==
                    Wrestler::RUN_RIGHT2 || state == Wrestler::STAND_RIGHT
                    || state == Wrestler::DASH_RIGHT)
                    w->setCurSpriteState(Wrestler::STAND_RIGHT);
                else if (state == Wrestler::RUN_LEFT1 || state ==
                    Wrestler::RUN_LEFT2 || state == Wrestler::STAND_LEFT ||
                    state == Wrestler::DASH_LEFT)
                    w->setCurSpriteState(Wrestler::STAND_LEFT);
                else if (state == Wrestler::RUN_UP1 || state ==
                    Wrestler::RUN_UP2 || state == Wrestler::STAND_UP || state
                    == Wrestler::DASH_UP)
                    w->setCurSpriteState(Wrestler::STAND_UP);
                else
                    w->setCurSpriteState(Wrestler::STAND_DOWN);
            }
        }
        // update projectile ball location and base direction
        else if (actors[i]->hasProjectile()) {
            Projectile *proj = dynamic_cast<Projectile*>(actors[i]);
            sf::Vector2f ball_pos = proj->getBallPos();
            sf::Vector2f player_pos = actors[0]->getPos();
            sf::Vector2f view_size = sf_view.getSize();
            Projectile::Dir dir = proj->getDir();
            if ((dir == Projectile::EAST || dir == Projectile::WEST) &&
                (!proj->hasShot() || ball_pos.x > player_pos.x+view_size.x/2 ||
                ball_pos.x+70 < player_pos.x-view_size.x/2)) {
                proj->shootBall();
                proj->setHasShot(true);
            }
            else if ((dir == Projectile::NORTH || dir == Projectile::SOUTH) &&
                (!proj->hasShot() || ball_pos.y > player_pos.y+view_size.y/2 ||
                ball_pos.y+70 < player_pos.y-view_size.y/2)) {
                proj->shootBall();
                proj->setHasShot(true);
            }
            else {
                if (dir == Projectile::NORTH)
                    proj->moveBall(0,-7);
                else if (dir == Projectile::EAST)
                    proj->moveBall(7,0);
                else if (dir == Projectile::SOUTH)
                    proj->moveBall(0,7);
                else
                    proj->moveBall(-7,0);
            }

            // check to see if we should change the base direction
            sf::Vector2f base_pos = proj->getPos();
            if (abs(player_pos.x-base_pos.x) > abs(player_pos.y-base_pos.y)) {
                if (player_pos.x > base_pos.x) {
                    proj->setBaseDir(Projectile::EAST);
                    proj->setState(Projectile::RIGHT);
                }
                else {
                    proj->setBaseDir(Projectile::WEST);
                    proj->setState(Projectile::LEFT);
                }
            }
            else {
                if (player_pos.y > base_pos.y) {
                    proj->setBaseDir(Projectile::SOUTH);
                    proj->setState(Projectile::DOWN);
                }
                else {
                    proj->setBaseDir(Projectile::NORTH);
                    proj->setState(Projectile::UP);
                }
            }
        }
    }

    updateDeaths(loc_map, actors, profile, lev_handler, num_bad_guys, wilhelm);
}

void setActorSpd(Wrestler* actor, int dir, LocationalMap &loc_map)
{
    //float acc = 0.2;
    float acc = actor->getAcceleration();

        // move left
        if (dir == 1) {
            actor->setVelocity(actor->getVelocity().x-acc,
            actor->getVelocity().y, loc_map.getLevelWidth(),
            loc_map.getLevelHeight());
        }

        // move right
        else if (dir == 3) {
            actor->setVelocity(actor->getVelocity().x+acc,
            actor->getVelocity().y, loc_map.getLevelWidth(),
            loc_map.getLevelHeight());
        }
        // move up
        if (dir == 0) {
            actor->setVelocity(actor->getVelocity().x,
            actor->getVelocity().y-acc, loc_map.getLevelWidth(),
            loc_map.getLevelHeight());
        }

        // move down
        else if (dir == 2) {
            actor->setVelocity(actor->getVelocity().x,
            actor->getVelocity().y+acc, loc_map.getLevelWidth(),
            loc_map.getLevelHeight());
        }
}

void getInputSetSpd(Collidable* wrestler, LocationalMap& loc_map,
std::vector<Collidable*>& actors, std::string ai_code, sf::Sound *sound)
//void getInputSetSpd(Wrestler* sumo)
{
    Wrestler *w = dynamic_cast<Wrestler*>(wrestler);

    // increase stamina
    w->increaseStamina();

    //Need to unfreeze the wrestler if enough frames (time) have passed for each special state
    if (w->getCurrentState() == Wrestler::DASH && w->getFrozenFrames() >= 30){
        w->setCurrentState(Wrestler::NORMAL);
    }
    if ((w->getCurrentState() == Wrestler::GRABBING || w->getCurrentState() == Wrestler::GRABBED) && w->getFrozenFrames() >= 240){
        w->setCurrentState(Wrestler::NORMAL);
    }
    if (w->getCurrentState() == Wrestler::THROWN && w->getFrozenFrames() >= 35){
        w->setCurrentState(Wrestler::NORMAL);
    }
    if (w->getCurrentState() == Wrestler::NOGRAB && w->getFrozenFrames() >= 35){
        w->setCurrentState(Wrestler::NORMAL);
    }
    //The grabber or grabbee may have been hit while in a grab, so the other may need to be broken out
    Wrestler* grabbed_wres = dynamic_cast<Wrestler*>(getActorById(w->getIDOfGrabbed(), actors));
    if (grabbed_wres != NULL &&
        ((w->getCurrentState() == Wrestler::GRABBING && grabbed_wres->getCurrentState() == Wrestler::NORMAL) ||
        (w->getCurrentState() == Wrestler::NORMAL && grabbed_wres->getCurrentState() == Wrestler::GRABBED))){
        w->setCurrentState(Wrestler::NORMAL);
        grabbed_wres->setCurrentState(Wrestler::NORMAL);
    }

    //Can't move, dash, or grab if in a special state
    if (w->getCurrentState() == Wrestler::NORMAL || w->getCurrentState() == Wrestler::NOGRAB){
        // dash
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && ai_code == "" &&
            (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))){
            w->useDash();
        }

        // grab: dash will have precedence over this
        else if (w->getCurrentState() == Wrestler::NORMAL && sf::Keyboard::isKeyPressed(sf::Keyboard::K) && ai_code == ""){
            grabProcedure(w, loc_map, actors, sound);
        }
        //Move if doing nothing else
        else{
            Wrestler::SpriteState state = w->getCurSpriteState();
            // move left
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) && ai_code == "")
                || ai_code == "left"){
                if (state != Wrestler::RUN_LEFT1 && state != Wrestler::RUN_LEFT2
                    && state != Wrestler::STAND_LEFT)
                    w->setCurSpriteState(Wrestler::RUN_LEFT1);
                else {
                    if (state == Wrestler::RUN_LEFT1) {
                        w->setCurSpriteState(Wrestler::STAND_LEFT);
                        w->setStep(true);
                    }
                    else if (state == Wrestler::RUN_LEFT2) {
                        w->setCurSpriteState(Wrestler::STAND_LEFT);
                        w->setStep(false);
                    }
                    else if (state == Wrestler::STAND_LEFT) {
                        if (w->getStep())
                            w->setCurSpriteState(Wrestler::RUN_LEFT2);
                        else
                            w->setCurSpriteState(Wrestler::RUN_LEFT1);
                    }
                }

                setActorSpd(w, 1, loc_map);
            }

            // move right
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) && ai_code ==
                "") || ai_code == "right") {
                if (state != Wrestler::RUN_RIGHT1 && state !=
                    Wrestler::RUN_RIGHT2 && state != Wrestler::STAND_RIGHT)
                    w->setCurSpriteState(Wrestler::RUN_RIGHT1);
                else {
                    if (state == Wrestler::RUN_RIGHT1) {
                        w->setCurSpriteState(Wrestler::STAND_RIGHT);
                        w->setStep(true);
                    }
                    else if (state == Wrestler::RUN_RIGHT2) {
                        w->setCurSpriteState(Wrestler::STAND_RIGHT);
                        w->setStep(false);
                    }
                    else if (state == Wrestler::STAND_RIGHT) {
                        if (w->getStep())
                            w->setCurSpriteState(Wrestler::RUN_RIGHT2);
                        else
                            w->setCurSpriteState(Wrestler::RUN_RIGHT1);
                    }
                }

                setActorSpd(w, 3, loc_map);
            }

            // move up
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) && ai_code == "")
                || ai_code == "up"){
                // only change the sprite if we're moving faster in this
                // direction
                if (abs(w->getVelocity().y) > abs(w->getVelocity().x)) {
                    if (state != Wrestler::RUN_UP1 && state != Wrestler::RUN_UP2
                        && state != Wrestler::STAND_UP) {
                        w->setCurSpriteState(Wrestler::RUN_UP1);
                    }
                    else {
                        if (state == Wrestler::RUN_UP1) {
                            w->setCurSpriteState(Wrestler::STAND_UP);
                            w->setStep(true);
                        }
                        else if (state == Wrestler::RUN_UP2) {
                            w->setCurSpriteState(Wrestler::STAND_UP);
                            w->setStep(false);
                        }
                        else if (state == Wrestler::STAND_UP) {
                            if (w->getStep())
                                w->setCurSpriteState(Wrestler::RUN_UP2);
                            else
                                w->setCurSpriteState(Wrestler::RUN_UP1);
                        }
                    }
                }
                setActorSpd(w, 0, loc_map);
            }

            // move down
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) && ai_code ==
                "") || ai_code == "down"){
                // only change sprite if we're going faster in this direction
                if (w->getVelocity().y > abs(w->getVelocity().x)) {
                    if (state != Wrestler::RUN_DOWN1 && state !=
                        Wrestler::RUN_DOWN2 && state != Wrestler::STAND_DOWN) {
                        w->setCurSpriteState(Wrestler::RUN_DOWN1);
                    }
                    else {
                        if (state == Wrestler::RUN_DOWN1) {
                            w->setCurSpriteState(Wrestler::STAND_DOWN);
                            w->setStep(true);
                        }
                        else if (state == Wrestler::RUN_DOWN2) {
                            w->setCurSpriteState(Wrestler::STAND_DOWN);
                            w->setStep(false);
                        }
                        else if (state == Wrestler::STAND_DOWN) {
                            if (w->getStep())
                                w->setCurSpriteState(Wrestler::RUN_DOWN2);
                            else
                                w->setCurSpriteState(Wrestler::RUN_DOWN1);
                        }
                    }
                }
                setActorSpd(w, 2, loc_map);
            }
        }
    }

    //Can only use throws if grabbing
    else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
             w->getCurrentState() == Wrestler::GRABBING && ai_code == ""){
        Wrestler* grabee = dynamic_cast<Wrestler*>(getActorById(w->getIDOfGrabbed(), actors));
        w->useThrow(grabee, loc_map.getLevelWidth(), loc_map.getLevelHeight());
    }

    w->incFrozenFrames();
}

//void setAISpd(Collidable* ai_sumo, LocationalMap& loc_map, std::vector<Collidable*>& actors)
//{
//
//    // get player's and ai's locations
//    sf::Vector2f player_pos = actors[0]->getPos();
//    sf::Vector2f ai_pos = ai_sumo->getPos();
//    Wrestler* ai_wrest = dynamic_cast<Wrestler*>(ai_sumo);
//
//    // move left
//    if (player_pos.x < ai_pos.x){
//        getInputSetSpd(ai_wrest, loc_map, actors, "left");
//    }
//
//    // move right
//    else if (player_pos.x > ai_pos.x){
//        getInputSetSpd(ai_wrest, loc_map, actors, "right");
//    }
//
//    // move up
//    if (player_pos.y < ai_pos.y){
//        getInputSetSpd(ai_wrest, loc_map, actors, "up");
//    }
//
//    // move down
//    else if (player_pos.y > ai_pos.y){
//        getInputSetSpd(ai_wrest, loc_map, actors, "down");
//    }
//}

std::vector<std::set<long int> > calcCollision(LocationalMap& loc_map,
                                               std::vector<Collidable*>& actors)
{
    std::vector<std::set<long int> > collision_sets = findFutureCollisions(loc_map, actors);
    std::vector<Collidable*> sel_actors;

    for(unsigned int i = 0; i < collision_sets.size(); i++){

        //Find the wrestlers according to their IDs
        std::set<long int>::iterator it;
        for (it = collision_sets[i].begin(); it != collision_sets[i].end(); ++it){
            sel_actors.push_back(getActorById(*it, actors));
        }

        //Calculate collision
        // v1 = (u1(m1-m2)+2m2u2)/m1+m2
        // v2 = (u2(m2-m1)+2m1u1)/m1+m2
        sf::Vector2f
        u1(sel_actors[0]->getVelocity());
        sf::Vector2f
        u2(sel_actors[1]->getVelocity());

        // now using weight of actor
        float m1 = sel_actors[0]->getWeight();
        float m2 = sel_actors[1]->getWeight();

        sf::Vector2f v1 = ((u1*(m1-m2))+2.f*m2*u2)/(m1+m2);
        sf::Vector2f v2 = ((u2*(m2-m1))+2.f*m1*u1)/(m1+m2);

        //Set new speeds
        sel_actors[0]->setVelocity(v1.x, v1.y, loc_map.getLevelWidth(),
        loc_map.getLevelHeight());
        sel_actors[1]->setVelocity(v2.x, v2.y, loc_map.getLevelWidth(),
        loc_map.getLevelHeight());

        //If either of these objects are wrestlers, return its state to NORMAL
        Wrestler* first = dynamic_cast<Wrestler*>(sel_actors[0]);
        Wrestler* second = dynamic_cast<Wrestler*>(sel_actors[1]);
        if (first && first->getCurrentState() != Wrestler::NORMAL && first->getCurrentState() != Wrestler::NOGRAB){
            first->setCurrentState(Wrestler::NORMAL);
        }
        if (second && second->getCurrentState() != Wrestler::NORMAL && second->getCurrentState() != Wrestler::NOGRAB){
            second->setCurrentState(Wrestler::NORMAL);
        }

        sel_actors.clear();
    }
    return collision_sets;
}


void calcProjectileCollision(std::vector<Collidable*>& actors, int level_w, int level_h)
{
    std::vector<Projectile*> projectiles;

    // get separate vector for projectile objects
    for (unsigned int i=0; i<actors.size(); i++) {
        if (actors[i]->hasProjectile()) {
            projectiles.push_back(dynamic_cast<Projectile*>(actors[i]));
        }
    }

    // calc projectile collisions
    for (unsigned int i=0; i<actors.size(); i++) {
        sf::RectangleShape
        actor_box(sf::Vector2f(actors[i]->getWidth(),actors[i]->getHeight()));
        actor_box.setPosition(actors[i]->getPos());
        sf::FloatRect actor_bounds = actor_box.getGlobalBounds();

        for (unsigned int j=0; j<projectiles.size(); j++) {
            sf::CircleShape ball;
            ball.setRadius(15);
            ball.setPosition(projectiles[j]->getBallPos().x+30,projectiles[j]->getBallPos().y+30);
            sf::FloatRect ball_bounds = ball.getGlobalBounds();

            if (actors[i]->getID() != projectiles[j]->getID() && actor_bounds.intersects(ball_bounds)) {
                sf::Vector2f u1(actors[i]->getVelocity());
                sf::Vector2f u2;
                // set projectile velocity based on its direction
                if (projectiles[j]->getDir() == Projectile::NORTH) {
                    u2.x = 0;
                    u2.y = -7;
                }
                else if (projectiles[j]->getDir() == Projectile::EAST) {
                    u2.x = 7;
                    u2.y = 0;
                }
                else if (projectiles[j]->getDir() == Projectile::SOUTH) {
                    u2.x = 0;
                    u2.y = 7;
                }
                else {
                    u2.x = -7;
                    u2.y = 0;
                }
                float m1 = actors[i]->getWeight();
                float m2 = 15;
                sf::Vector2f v1 = (u1*(m1-m2)+2.f*m2*u2)/(m1+m2);

                // update actors velocity and reset projectile
                actors[i]->setVelocity(v1.x,v1.y,level_w, level_h);
                projectiles[j]->shootBall();

//                // add to collision sets if not already there
//                for (unsigned int cs=0; cs<collision_sets.size(); cs++) {
//                    if (collision_sets[cs].find(actors[i]->getID()) ==
//                        collision_sets[cs].end()) {
//                        std::set<long int> new_set;
//                        new_set.insert(actors[i]->getID());
//                        collision_sets.push_back(new_set);
//                    }
//                }
            }
        }
    }
}
