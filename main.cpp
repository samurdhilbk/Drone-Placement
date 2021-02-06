#include <vector>
#include <iostream>
#include <random>
#include <fstream>
#include <queue>

//Used to sample from a Truncated Normal Distribution.
//From: https://people.sc.fsu.edu/~jburkardt/cpp_src/truncated_normal/truncated_normal.html
#include "truncated_normal.h"

using namespace std;

typedef long double ld;
typedef pair<ld, ld> pr;

const ld eps = 1e-16; //binary-search sensitivity

//A rectangle
struct Rect{
    ld x; //x-coordinate of top-left corner
    ld y; //y-coordinate of top-left corner
    ld dx; //x-width
    ld dy; //y-width
};

std::random_device rd;
std::mt19937 e2(rd());

//Split rectangle along x-width
pair<Rect, Rect> split_rect_x(Rect rect, ld D, ld sigma){
    if(rect.dx < 2.0*D){
        throw("Invalid rect to x split");
    }

    int seed = 123456789;
    ld split_dx;

    //find the splitting point of the rectangle
    split_dx = truncated_normal_ab_sample ( 0.5, sigma, 0, 1, seed );

    Rect rect1;
    Rect rect2;

    rect1.dy = rect.dy;
    rect2.dy = rect.dy;

    rect1.dx = D + split_dx*(rect.dx - 2*D);
    rect2.dx = rect.dx - rect1.dx;

    rect1.x = rect.x;
    rect1.y = rect.y;

    rect2.x = rect.x + rect1.dx;
    rect2.y = rect.y;

    return make_pair(rect1, rect2);
}

//Split rectangle along y-width
pair<Rect, Rect> split_rect_y(Rect rect, ld D, ld sigma){
    if(rect.dy < 2*D){
        throw("Invalid rect to y split");
    }

    int seed = 123456789;
    ld split_dy;

    //find the splitting point of the rectangle
    split_dy = truncated_normal_ab_sample ( 0.5, sigma, 0, 1, seed );

    Rect rect1;
    Rect rect2;

    rect1.dx = rect.dx;
    rect2.dx = rect.dx;

    rect1.dy = D + split_dy*(rect.dy - 2*D);
    rect2.dy = rect.dy - rect1.dy;

    rect1.y = rect.y;
    rect1.x = rect.x;

    rect2.y = rect.y + rect1.dy;
    rect2.x = rect.x;

    return make_pair(rect1, rect2);
}

// can rect be split to two rectangles each able to hold a circle
bool can_split(Rect rect, ld D){
    return (rect.dx >= 2*D || rect.dy >= 2* D);
}

// split rect to two such that each of the two resulting rectangles are able to hold a circle
pair<Rect, Rect> split_rect(Rect rect, ld D, ld sigma){
    const std::vector<Rect> empty_vec;
    if(!can_split(rect, D)){
        throw("Invalid rect to split");
    }
    else if(rect.dx >= 2*D && rect.dy >= 2*D){
        //if rect can be split along either dimension, choose which dimension to split randomly
        std::uniform_real_distribution<> toss_dist(0.0, 1.0);
        ld toss = toss_dist(e2);
        if(toss>0.5){
            return split_rect_x(rect, D, sigma);
        }
        else{
            return split_rect_y(rect, D, sigma);
        }
    }
    else if(rect.dx >= 2*D){
        return split_rect_x(rect, D, sigma);
    }
    else if(rect.dy >= 2*D){
        return split_rect_y(rect, D, sigma);
    }
    else{
        throw("Something has gone wrong. This line should be unreachable.");
    }
}

//Compare two rects. Larger rect (in terms of max(width, height)) will compare >
struct rect_compare{
public:
    bool operator()(Rect &r1, Rect &r2)
    {
        ld mx_dim_1 = max(r1.dx, r1.dy);
        ld mx_dim_2 = max(r2.dx, r2.dy);
        return mx_dim_1<mx_dim_2;
    }
};

//Try to split L x L square to N non-overlapping rectangles such that
//each rectangle satisfies min(width, height) >= 2*r
//Use given sigma to control tightness of fit (amount of randomness)
vector<Rect> split_to_rects_helper(int N, ld r, ld L, ld sigma){
    const std::vector<Rect> empty_vec;

    ld d = 2*r;

    Rect init;
    init.x = 0;
    init.y = 0;
    init.dx = L;
    init.dy = L;

    //maximum priority queue to hold rects that could further be split
    //Larger rectangles will be split first
    priority_queue<Rect, std::vector<Rect>, rect_compare> curr_eligible;

    //vector to hold rects that cannot be split further
    vector<Rect> curr_ineligible;

    if(can_split(init, d)){
        curr_eligible.push(init);
    }
    else{
        curr_ineligible.push_back(init);
    }

    while((curr_eligible.size() + curr_ineligible.size()) < N && !curr_eligible.empty()){
        Rect curr_rect = curr_eligible.top();
        pair<Rect, Rect> prec = split_rect(curr_rect, d, sigma);
        curr_eligible.pop();

        if(can_split(prec.first, d)) curr_eligible.push(prec.first);
        else curr_ineligible.push_back(prec.first);

        if(can_split(prec.second, d)) curr_eligible.push(prec.second);
        else curr_ineligible.push_back(prec.second);
    }

    vector<Rect> final;
    while(!curr_eligible.empty()){
        final.push_back(curr_eligible.top());
        curr_eligible.pop();
    }
    for(Rect rect: curr_ineligible) final.push_back(rect);

    return final;
}

//Given a rectangle, find a random location inside rectangle such that the circle fits completely inside
pr fit_circle_inside_rect(Rect rect, ld D){
    std::uniform_real_distribution<> dist_dx(0.0, rect.dx - D);
    std::uniform_real_distribution<> dist_dy(0.0, rect.dy - D);

    ld dx = dist_dx(e2);
    ld dy = dist_dy(e2);

    return make_pair(rect.x + D/2 + dx, rect.y + D/2 + dy);
}

//Given a set of N rectangles, fit a circle randomly inside each rectangle
vector<pr> pack_circles(vector<Rect> rects, int N, ld r){
    vector<pr> center_coords;
    for(Rect rect: rects) center_coords.push_back(fit_circle_inside_rect(rect, 2*r));
    return center_coords;
}

//Try to split L x L square to N non-overlapping rectangles such that
//each rectangle satisfies min(width, height) >= 2*r
//Use maximum sigma possible to do the fitting
vector<Rect> split_to_rects(int N, ld r, ld L){
    ld sigma_l = 0;
    ld sigma_r = 1e18;

    ld sigma;
    vector<Rect> ret;

    //binary search for the largest sigma that allows us to find N rectangles
    while(abs(sigma_l-sigma_r)>eps){

        sigma = (sigma_r + sigma_l) / 2;

        ret = split_to_rects_helper(N, r, L, sigma);

        if(ret.size() < N){
            sigma_r = sigma;
        }
        else{
            sigma_l = sigma;
        }
    }

    return ret;

}

int main(){

    /***********************************************************************************************/
    /* Set parameters here */

    int N=1000; //Number of drones
    ld r=1.0; //Radius of each drone
    ld L=200; //Length of each side of the square

    bool print_circle_coordinates_to_console = true;
    bool print_circle_coordinates_to_file = true; //file will contain N, r, L on first line and following lines
    // will contain coordinates of the center of each circle

    bool print_rect_coordinates_to_console = true;
    bool print_rect_coordinates_to_file = true; //file will contain N, r, L on first line and following lines
    // will contain locations and sizes of bounding rectangles

    string file_path_rect = "../output_rects.txt";
    string file_path_circle_coordinates = "../output_circles.txt";


    /***********************************************************************************************/

    vector<Rect> rects = split_to_rects(N, r, L);

    if(rects.size() < N){
        cout<<"Couldn't find a solution. Try increasing the size of the square."<<endl;
        return 0;
    }
    else{
        cout<<"Found a solution!"<<endl;
    }

    vector<pr> center_coords = pack_circles(rects, N, r);


    if(print_rect_coordinates_to_console) {
        cout<<"Printing bounding rectangles .... (top-left corner-x, top-left corner-y, width, height)"<<endl;
        for (Rect rect: rects) {
            cout << rect.x << " " << rect.y << " " << rect.dx << " " << rect.dy << endl;
        }
    }

    if(print_circle_coordinates_to_console){
        cout<<"Printing center coordinates of circles (drones) .... (center-x, center-y)"<<endl;
        for(pr coord: center_coords){
            cout<<coord.first<<" "<<coord.second<<endl;
        }
    }


    if(print_rect_coordinates_to_file) {
        ofstream myfile(file_path_rect);
        if (myfile.is_open()) {
            myfile << N << " " << r << " " << L << endl;
            for (Rect rect: rects) {
                myfile << rect.x << " " << rect.y << " " << rect.dx << " " << rect.dy << endl;
            }
            myfile.close();
            cout<<"Printed bounding rectangles to file .... (top-left corner-x, top-left corner-y, width, height)"<<endl;
        }
    }

    if(print_circle_coordinates_to_file){
        ofstream myfile1(file_path_circle_coordinates);
        if (myfile1.is_open())
        {
            myfile1<<N<<" "<<r<<" "<<L<<endl;
            for(pr coord: center_coords){
                myfile1<<coord.first<<" "<<coord.second<<endl;
            }
            myfile1.close();
            cout<<"Printed center coordinates of circles (drones) to file .... (center-x, center-y)"<<endl;
        }
    }


	return 0;
}