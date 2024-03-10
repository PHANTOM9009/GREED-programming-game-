//this file contains the first algorithm for min-max testing by using a mini brain for combat scenarios:
#include<iostream>
#include<vector>
#include<deque>
#include<unordered_map>
/*
* this algorithm could be used to predict the win or lose in a close scenario, keep the length of the tree down by diving the health, ammo and money with
* by a factor of 10 so that the result will be prompt, the result will be same if you don't reduce the lenght, since the extra iterations are only for 
* formality. so this could be used to tell if you will win or loose with your opponent, this is assuming that you are fighting with only one opponent.
* 
*/
using namespace std;
enum actions {
	FIRE, ESCAPE,UAMMO_FIRE,UAMMO_HEALTH //OR THERE can be a combination of these actions
};
class state
{
	//basic class to represent the state for every ship-
public:
	int id;//ship Id
	double gold;
	double health;
	double fuel;
	double ammo;
	bool isInRadius;
	actions prev_action;//action taken by the ship in the previous state
	int result;//result that will come from the bottom of the search tree like in min max
	int depth;

	state()
	{

	}
	state(int i, double g, double h, double f, double a, bool is)
	{
		id = i;
		gold = g;
		health = h;
		fuel = f;
		ammo = a;
		isInRadius = is;
	}
};
class node
{
	//node for each tree
public:
	bool action_taker_id;//0 for max and 1 for min
	vector<state> states;//states for the two ships, the fist is you and the second is your enemy
	vector<node> children;//children of the node
	int alpha;
	int beta;
	int result;
	node()
	{
		alpha = INT_MIN;
		beta = INT_MAX;
	}

};
long double total_states = 0;
vector<actions> max_decision;
vector<actions> min_decision;
int createDecisionTree(node &start,int my_id,int ad_id,int real_ad_id,int depth)//pass my_id as 0 and ad_id and 1 and real_ad_id in the same order
{
	//handling the base cases
	if (start.states[0].health <= 0)
	{
		//max has died hence min wins
		start.result = -1;
		return -1;
	}
	else if (start.states[1].health <= 0)
	{
		//min has died hence max wins
		start.result = 1;
		return 1;
	}
	//tie will happen when the agent has left the radius of the adversary
	if (!start.states[my_id].isInRadius || !start.states[ad_id].isInRadius)
	{
		start.result = 0;
		return 0;
	}
	//it will have the starting state and will create the min max tree
	actions action;
	actions final_action;
	for (int i = 1; i <= 3; i++)
	{
		if (start.alpha >= start.beta)
		{
			break;
		}
		//taking the action and calling this function recursively
		int gone = 0;
		node n;
		n.alpha = start.alpha;
		n.beta = start.beta;
		if (i == 1 && start.states[my_id].isInRadius && start.states[my_id].ammo>0)//take the action of firing
		{
			action = actions::FIRE;
			//creating a new child node now filling it up with data
			n.action_taker_id=!start.action_taker_id;//next chance will of the other player
			n.states = start.states;//copying the states of the parent node
			//now changing the states of the child node
			n.states[my_id].ammo = n.states[my_id].ammo - 1;//decreasing the ammo
			n.states[my_id].prev_action = FIRE;//setting the previous action
			//now adding the child node to the parent node
			n.states[ad_id].health-=5;//decreasing the health of the adversary
			
			gone = 1;
		}
		else if (i == 2 && start.states[my_id].isInRadius &&start.states[my_id].ammo<=5 && start.states[my_id].gold >= 5)
		{
			//upgrading the ammo by 5 and then firing at the adversary
			action = actions::UAMMO_FIRE;
			n.action_taker_id = !start.action_taker_id;//next chance will of the other player
			n.states = start.states;//copying the states of the parent node
			//now changing the states of the child node
			n.states[my_id].ammo += 5;
			n.states[my_id].gold -= 5;
			n.states[my_id].ammo = n.states[my_id].ammo - 1;//decreasing the ammo
			n.states[my_id].prev_action = FIRE;//setting the previous action
			//now adding the child node to the parent node
			n.states[ad_id].health -= 5;//decreasing the health of the adversary
			
			gone = 1;
		}
		else if (i == 3 && start.states[my_id].isInRadius && start.states[my_id].ammo > 0 && start.states[my_id].gold >= 5 * 5 && start.states[my_id].health<=5)
		{
			action = actions::UAMMO_HEALTH;
			//creating a new child node now filling it up with data
			n.action_taker_id = !start.action_taker_id;//next chance will of the other player
			n.states = start.states;//copying the states of the parent node
			//now changing the states of the child node
			n.states[my_id].ammo = n.states[my_id].ammo - 1;//decreasing the ammo
			n.states[my_id].prev_action = FIRE;//setting the previous action
			//now adding the child node to the parent node
			n.states[ad_id].health -= 5;//decreasing the health of the adversary

			n.states[my_id].health += 5;
			n.states[my_id].gold -= 25;
			gone = 1;
		}
		/*
		else if (i == 2 && start.states[my_id].isInRadius)///running away only if you have not already ran away
		{
			action = actions::ESCAPE;
			n.action_taker_id = !start.action_taker_id;//next chance will of the other player
			n.states = start.states;//copying the states of the parent node
			n.states[my_id].isInRadius = false;
			n.states[ad_id].isInRadius = false;
			if (!n.action_taker_id)
			{
				n.result = INT_MIN;
			}
			else
			{
				n.result = INT_MAX;
			}
			gone = 1;
		}
		*/
		if (!n.action_taker_id)
		{
			n.result = INT_MIN;
		}
		else
		{
			n.result = INT_MAX;
		}
		if (gone == 1)
		{
			total_states++;
			start.children.push_back(n);
			if (start.action_taker_id == false)//for max
			{
				int score = createDecisionTree(start.children[start.children.size() - 1], ad_id, my_id, real_ad_id,depth+1);
				start.alpha = max(start.alpha, score);
				if (start.result < score)
				{
					start.result = score;
					final_action = action;

				}
			}
			if (start.action_taker_id)//for min
			{
				int score = createDecisionTree(start.children[start.children.size() - 1], ad_id, my_id, real_ad_id,depth+1);
				start.beta = min(start.beta, score);
				if (start.result > score)
				{
					start.result = score;
					final_action = action;
				}
			}
		}
				
	}
	if (!start.action_taker_id)
	{
		max_decision.push_back(final_action);
	}
	if (start.action_taker_id)
	{
		min_decision.push_back(final_action);
	}
	return start.result;

	
}
void displayTree(node& state,int level)
{
	for (int i = 1; i <= level; i++)
	{
		cout << "\t";
	}
	cout << "state is==>" << state.action_taker_id << " state 0 data Health,ammo,isRadius is==>" << state.states[0].health << " " << state.states[0].ammo << " " << state.states[0].isInRadius;
	cout << "state 1 data is==>" << state.states[1].health << " " << state.states[1].ammo<<" "<<state.states[1].isInRadius << " the result is==>" << state.result;
	cout << endl;
	for (int i = 0; i < state.children.size(); i++)
	{
		cout << endl<<"\t";
		displayTree(state.children[i],level+1);
	}
	
}
int main()
{
	state s0(0, 200, 100, 1, 5, true);
	state s1(1, 80, 150, 1, 5, true);
	node first;
	first.action_taker_id = false;
	first.states.push_back(s0);
	first.states.push_back(s1);
	first.result = INT_MIN;//starting from the max
	cout << "\n the result of tree is==>" << createDecisionTree(first, 0, 1, 1,0);
	cout << "\n total states are==>" << total_states;
	for (int i = 0; i < max_decision.size(); i++)
	{
		cout << (int)max_decision[i] <<" ";
	}
	/*
	cout << "\n the tree is==>\n";
	displayTree(first,0);
	cout << "\n the max ation is==>";
	for (int i = 0; i < max_decision.size(); i++)
	{
		cout << (int)max_decision[i] << endl;
	}
	cout << "\n the min action is==>" << endl;
	for (int i = 0; i < min_decision.size(); i++)
	{
		cout << (int)min_decision[i] << endl;

	}
	*/
}



