#include<bits/stdc++.h>
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
using namespace std;

void clrscr()
{
   system("cls");
}

#define ALPHABET_SIZE (26)

#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

#define rD (rand() / (double) RAND_MAX)		// random decimal

#define popsize 200
#define ngenerations 400
#define elite 1
const double pCross = 0.85;			// probability of crossover
const double pMutate = 0.1;			// probability of mutation
struct chromosome
{
	unsigned int genes;
	int fitness;
	int weight;
	chromosome() { }
};

int ngenes,s;		// number of items, size of the knapsack
int w[55];		// weights
int v[55];		// values
double cdf[popsize];	// cumulative probability distribution

chromosome cPopulation[popsize];	// current population
chromosome nPopulation[popsize];	// next population

void init()
{
	memset(cPopulation, 0 , sizeof(cPopulation));
	srand(time(0));
	for(int i = 0; i < popsize; i++)
	{
		int cW = 0;						// current weight
		for(int j = 0; j < ngenes && cW < s; j++)
		{
			if(rand() % 2 && cW + w[j] <= s)
			{
				cPopulation[i].genes |= (1<<j);
				cW += w[j];
			}
		}
	}
}

void calcFitness()
{
	int totalFitness = 0;
	memset(cdf,0,sizeof(cdf));
	for(int i = 0; i < popsize; i++)
	{
		cPopulation[i].fitness = 0;
		cPopulation[i].weight = 0;
		for(int j = 0; j < ngenes; j++)
		{
			cPopulation[i].fitness += ((cPopulation[i].genes >> j) & 1) * v[j];
			cPopulation[i].weight  += ((cPopulation[i].genes >> j) & 1) * w[j];
		}
		if(cPopulation[i].weight > s) cPopulation[i].fitness = 0;					// shouldn't happen since all cases are handled, but just in case
		totalFitness += cPopulation[i].fitness;
	}
	for(int i = 0; i < popsize; i++)									// create CDF
	{
		double probability_of_i = cPopulation[i].fitness / (double) totalFitness;
		cdf[i] = (i > 0 ? cdf[i-1] + probability_of_i : probability_of_i);
	}
}

bool valid(chromosome* pC)
{
	int totalW = 0;
	for(int i=0; i < ngenes; i++)
		totalW += ((pC->genes >> i) & 1) * w[i];
	return totalW <= s;
}

void evolve()
{
	int cInd = 0;
	if(elite)
	{
		pair<int, int> best (0,0), secondBest (0,0);								// elitism, let the two fittest chromosomes survive with every generation
		for(int i = 0; i < popsize; i++)
		{
			if(cPopulation[i].fitness >= best.second)
			{
				secondBest = best;
				best.first = i;
				best.second = cPopulation[i].fitness;
			}
			else if(cPopulation[i].fitness > secondBest.second)
			{
				secondBest.first = i;
				secondBest.second = cPopulation[i].fitness;
			}
		}
		nPopulation[cInd++] = cPopulation[best.first];
		nPopulation[cInd++] = cPopulation[secondBest.first];
	}

	// begin natural selection

	while(cInd < popsize)
	{
		chromosome p1 = cPopulation[upper_bound(cdf, cdf + popsize, rD) - cdf]; //first parent
		chromosome p2 = cPopulation[upper_bound(cdf, cdf + popsize, rD) - cdf]; //second parent
		chromosome c1 = p1;
		chromosome c2 = p2;
		nPopulation[cInd++] = p1;
		nPopulation[cInd++] = p2;
		bool willCross = (rD <= pCross);
		if(willCross)
		{
			int crossover_point = rand() % ngenes;
			if(crossover_point == 0) crossover_point++;
			else if(crossover_point == ngenes) crossover_point--;
			chromosome c1 = p1;
			chromosome c2 = p2;
			for(int i = crossover_point + 1; i < ngenes; i++)
			{
				if(p1.genes & (1<<(i-1))) 		// if the i'th bit is on in parent 1, it should be on in child 2
					c2.genes |= 1<<(i-1);
				else			 		// if the i'th bit is off in parent 1, it should be off in child 2
					c2.genes &= ~(1<<(i-1));

				if(p2.genes & (1<<(i-1))) 		// if the i'th bit is on in parent 2, it should be on in child 1
					c1.genes |= 1<<(i-1);
				else			 		// if the i'th bit is off in parent 2, it should be off in child 1
					c1.genes &= ~(1<<(i-1));
			}
		}
		if(valid(&c1) && cInd < popsize) nPopulation[cInd++] = c1;
		if(valid(&c2) && cInd < popsize) nPopulation[cInd++] = c2;
	}

	// end natural selection
	// begin random mutation

	for(int i = 0; i < popsize; i++)
	{
		if(rD <= pMutate)
		{
			int flip = rand() % ngenes;			// random index to mutate (flip)
			chromosome mutated = nPopulation[i];
			mutated.genes ^= (1<<flip);
			if(valid(&mutated)) nPopulation[i] = mutated;
		}
	}

	// end random mutation
}



struct TrieNode
{
	struct TrieNode *children[ALPHABET_SIZE];
	bool isWordEnd;
};

struct TrieNode *getNode(void)
{
	struct TrieNode *pNode = new TrieNode;
	pNode->isWordEnd = false;

	for (int i = 0; i < ALPHABET_SIZE; i++)
		pNode->children[i] = NULL;

	return pNode;
}

void insert(struct TrieNode *root, const string key)
{
	struct TrieNode *pCrawl = root;

	for (int level = 0; level < key.length(); level++)
	{
		int index = CHAR_TO_INDEX(key[level]);

		if (!pCrawl->children[index])
			pCrawl->children[index] = getNode();

		pCrawl = pCrawl->children[index];
	}
	pCrawl->isWordEnd = true;
}

bool search(struct TrieNode *root, const string key)
{
	int length = key.length();

	struct TrieNode *pCrawl = root;

	for (int level = 0; level < length; level++)
	{
		int index = CHAR_TO_INDEX(key[level]);

		if (!pCrawl->children[index])
			return false;

		pCrawl = pCrawl->children[index];
	}

	return (pCrawl != NULL && pCrawl->isWordEnd);
}
bool isLastNode(struct TrieNode* root)
{
	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (root->children[i])
			return 0;
	return 1;
}

void suggestionsRec(struct TrieNode* root, string currPrefix)
{
	if (root->isWordEnd)
	{
		cout << currPrefix;
		cout << endl;
	}


	if (isLastNode(root))
		return;

	for (int i = 0; i < ALPHABET_SIZE; i++)
	{
		if (root->children[i])
		{
			currPrefix.push_back(97 + i);

			suggestionsRec(root->children[i], currPrefix);
		}
	}
}


int printAutoSuggestions(TrieNode* root, const string query)
{
	struct TrieNode* pCrawl = root;

	int level;

	int n = query.length();

	for (level = 0; level < n; level++)
	{
		int index = CHAR_TO_INDEX(query[level]);
		if (!pCrawl->children[index])
			return 0;

		pCrawl = pCrawl->children[index];
	}
	bool isWord = (pCrawl->isWordEnd == true);

	bool isLast = isLastNode(pCrawl);

	if (isWord && isLast)
	{
		cout << query << endl;
		return -1;
	}
	if (!isLast)
	{
		string prefix = query;
		suggestionsRec(pCrawl, prefix);
		return 1;
	}
}


void display()
 {
   fstream ip ("newcvs.csv");

    if(!ip.is_open()) std::cout<<"Error"<<endl;

    int i=0;
    vector<string>alphaf;
    vector<string>alphal;
    vector<string>numb;
    string fname,lname;
    string number;

    cout<<"Spaceship-Name"<<"\t\t\tPlanets and commets visited"<<"\t\tYear"<<endl<<endl;
    while(ip.good())
   {
    getline(ip,fname,',');
    getline(ip,lname,',');
    getline(ip,number,'\n');
    alphaf.push_back(fname);
    alphal.push_back(lname);
    numb.push_back(number);
    cout<<fname<<"\t\t\t\t"<<lname<<"\t\t\t\t"<<number<<endl<<endl;

   }
   ip.close();
}
selection()
{
    int c;
	cout<<"Enter the number of space-ships "<<endl;
	cin >> c;
	while(c--)
	{   cout<<"Enter capacity:";
        cin>> s;
	    cout<<"Enter number of items:";
		cin >> ngenes;

		for(int i = 0; i < ngenes; i++){
			cout<<"Enter weight:";
			cin >> w[i];
			cout<<"Enter nutrient value:";
			cin>> v[i];
		}
		init();
		for(int i = 0; i < ngenerations; i++)
		{
			calcFitness();							// calculates fitness for the current generation and creates the CDF table
			evolve();							    // natural selection, crossover, and random mutation
			memcpy(cPopulation, nPopulation, sizeof(nPopulation));		// replace current population with the new population
		}
		int globalOptimum = 0, gOInd = 0;
		calcFitness();
		for(int i = 0; i < popsize; i++)
		{
			if(cPopulation[i].fitness > globalOptimum)
			{
				globalOptimum = cPopulation[i].fitness;
				gOInd = i;
			}
		}
		cout<<"items selected"<<"\t=\t"<<"total nutrient value"<<endl;
		for(int i = 0; i < ngenes; i++)
			cout << ((cPopulation[gOInd].genes >> i) & 1);
		cout << "\t\t=\t" << globalOptimum << endl;

	}
}
void search1()
{
    fstream ip ("newcvs.csv");

    if(!ip.is_open()) std::cout<<"Error"<<endl;
    int i=0;
    vector<string>alpha;
    vector<string>alpha1;
    vector<string>alpha3;

    string fname,lname;
    string number,x;

    int found=0;

    while(ip.good())
    {
        getline(ip,fname,',');
        getline(ip,lname,',');
        getline(ip,number,'\n');

        alpha.push_back(fname);
        alpha1.push_back(lname);
        alpha3.push_back(number);
    }
    ip.close();
    int a,n = alpha.size();
    string asdf;

    struct TrieNode* root = getNode();

	for (int i=0; i<n; i++)
    {

		insert(root, alpha[i]);

    }
    cout<<"if you know the name enter 1 else enter 0"<<endl;
    cin>>a;

    if(a==0)
    {
    cout<<"enter the First name (or first 3-4 letter)"<<endl;
    cin>>x;

	int comp1 = printAutoSuggestions(root,x);

	if (comp1 == -1)
		cout << "No other strings found with this prefix\n";

	else if (comp1 == 0)
		cout << "No string found with this prefix\n";

    }

        cout<<"Enter the first name"<<endl;
        cin>>asdf;
        cout<<endl;
        cout<<"Spaceship-Name"<<"\t\t\tPlanets and commets visited"<<"\t\tYear"<<endl<<endl;
        for(int i=0;i<n;i++)
        {
            if(asdf==alpha[i])
            {

                cout<<alpha[i]<<"\t\t\t\t"<<alpha1[i]<<"\t\t\t\t"<<alpha3[i]<<endl;
                found++;
            }
        }

            if(!found)
            {
                cout<<"No match found!!!!"<<endl;
            }


}


void login()
{
	int a=0,i=0;
    char uname[10],c=' ';
    char pword[10],code[10];

    do
{

    cout<<"\n  **************************  LOGIN FORM  **************************  ";
    cout<<" \n                       ENTER USERNAME:-";
	cin>>uname;
	cout<<" \n                       ENTER PASSWORD:-";

	while(i<4)
	{
	    pword[i]=_getch();
	    c=pword[i];
	    cout<<"*";
	    i++;
	}
	_getch();
	pword[i]='\0';

	i=0;
		if((strcmpi(uname,"user")==0) && (strcmpi(pword,"pass")==0) )
	{
	cout<<"  \n\n\n       WELCOME TO OUR SYSTEM !!!! LOGIN IS SUCCESSFUL";
	cout<<"\n\n\n\t\t\t\tPress any key to continue...";

	break;
	}

	else
	{
		cout<<"\n        SORRY    !!!!  LOGIN IS UNSUCESSFUL";
		a++;
		_getch();
		clrscr();
	}
}
	while(a<=3);

	if (a>3)
	{
		cout<<"\nSorry you have entered the wrong username and password for two times!!!";
		}
		system("cls");
}

 main()
{

clrscr();
int choice;
int t;

int opt;

    	time_t time_;

	time(&time_);
    system("COLOR B");
    startmenu:
   cout<<"\t\t\t\t\t\t\t\t\t\t"<<ctime(&time_);
       cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::";
       cout<<"\n\t\t\t\t\t::                                 ::";
       cout<<"\n\t\t\t\t\t::     ***********************     ::";
       cout<<"\n\t\t\t\t\t::     *                     *     ::";
       cout<<"\n\t\t\t\t\t::     *      WELCOME TO     *     ::";
       cout<<"\n\t\t\t\t\t::     *       SPACE-EX      *     ::";
       cout<<"\n\t\t\t\t\t::     *    TRANSPORTATION   *     ::";
       cout<<"\n\t\t\t\t\t::     *                     *     ::";
       cout<<"\n\t\t\t\t\t::     ***********************     ::";
       cout<<"\n\t\t\t\t\t::                                 ::";
       cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::\n\n";

cout<<"\t\t\t\t\t\tSELECT YOUR IDENTITY"<<endl;
cout<<"\t\t\t\t\t\t1.USER"<<endl;
cout<<"\t\t\t\t\t\t2.ADMINISTRATOR"<<endl;

cout<<"\t\t\t\t\t\tCHOICE - ";
cin>>opt;

clrscr();

switch(opt)
{
    case 1:
        up1:

           cout<<"\t\t\t\t\t\t\t\t\t\t"<<ctime(&time_);
       cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::";
       cout<<"\n\t\t\t\t\t::                                 ::";
       cout<<"\n\t\t\t\t\t::     ***********************     ::";
       cout<<"\n\t\t\t\t\t::     *                     *     ::";
       cout<<"\n\t\t\t\t\t::     *      WELCOME TO     *     ::";
       cout<<"\n\t\t\t\t\t::     *       SPACE-EX      *     ::";
       cout<<"\n\t\t\t\t\t::     *    TRANSPORTATION   *     ::";
       cout<<"\n\t\t\t\t\t::     *                     *     ::";
       cout<<"\n\t\t\t\t\t::     ***********************     ::";
       cout<<"\n\t\t\t\t\t::                                 ::";
       cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::\n\n";

       cout<<endl<<endl;

    cout<<"\t\t\t\t\t\tWhat would you like to do - "<<endl;
    cout<<"\t\t\t\t\t\t1.Search record"<<endl;
	cout<<"\t\t\t\t\t\t2.Display record"<<endl;
	cout<<"\t\t\t\t\t\t3.Exit"<<endl;

    cout<<"\t\t\t\t\t\tEnter choice - ";
    cin>>t;

    clrscr();
     switch(t)
      {
	case 1: clrscr();
            search1();
            _getch();
            clrscr();
	goto up1;

	break;

	case 2: display();
        cout<<endl<<endl<<endl;
        _getch();
        clrscr();
	goto up1;

	break;

	case 3: clrscr();
            cout<<"\n\n\n\n\n\n\t\t\t\t\t\t Thank You";
            _getch();
            clrscr();
        goto startmenu;

        break;

    default :   clrscr();
                cout<<"\n\n\n\n\n\n\t\t\t\t\t\t wrong choice";
                _getch();
                clrscr();
    goto up1;


 }
    case 2:
            login();
            up2:
            cout<<"\t\t\t\t\t\t\t\t\t\t"<<ctime(&time_);
            cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::";
            cout<<"\n\t\t\t\t\t::                                 ::";
            cout<<"\n\t\t\t\t\t::     ***********************     ::";
            cout<<"\n\t\t\t\t\t::     *                     *     ::";
            cout<<"\n\t\t\t\t\t::     *      WELCOME TO     *     ::";
            cout<<"\n\t\t\t\t\t::     *       SPACE-EX      *     ::";
            cout<<"\n\t\t\t\t\t::     *    TRANSPORTATION   *     ::";
            cout<<"\n\t\t\t\t\t::     *                     *     ::";
            cout<<"\n\t\t\t\t\t::     ***********************     ::";
            cout<<"\n\t\t\t\t\t::                                 ::";
            cout<<"\n\t\t\t\t\t:::::::::::::::::::::::::::::::::::::\n\n";

            cout<<endl<<endl;

            cout<<"\t\t\t\t\t\tWhat would you like to do - "<<endl;
            cout<<"\t\t\t\t\t\t1.selection of item"<<endl;
            cout<<"\t\t\t\t\t\t2.Search"<<endl;
            cout<<"\t\t\t\t\t\t3.Display"<<endl;
            cout<<"\t\t\t\t\t\t4.Exit"<<endl;

            cout<<"\t\t\t\t\t\tEnter choice - ";
            cin>>choice;

            clrscr();
     switch(choice)
      {

	case 1: clrscr();
            selection();
            _getch();
            clrscr();
            goto up2;

	break;

	case 2: clrscr();
            search1();
            _getch();
            clrscr();
	goto up2;

	break;

	case 3: display();
            cout<<endl<<endl<<endl;
            _getch();
            clrscr();
	goto up2;

	break;

	case 4: clrscr();
            cout<<"\n\n\n\n\n\n\t\t\t\t\t\tThank You"<<endl;
            _getch();
            clrscr();
	goto startmenu;

	break;

	default:    clrscr();
                cout<<"\n\n\n\n\n\n\t\t\t\t\t\ttwrong choice";
                _getch();
                clrscr();
            goto up2;
       }

    default:    clrscr();
                cout<<"\n\n\n\n\n\n\t\t\t\t\t\twrong choice"<<endl;
                _getch();
                clrscr();
            goto startmenu;
}

}
