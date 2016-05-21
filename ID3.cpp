// ID3.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

// global class attribute
vector <string> class_atb;
fstream out("output.txt", ios::out);

vector <string> split(string _sStr, char delimit)
{
	vector <string> splitted;
	stringstream ss(_sStr);
	string tok;

	while (getline(ss, tok, delimit))
		splitted.push_back(tok);

	return splitted;
}

struct ClassCount
{
	string sClassName;
	int iCount;

	ClassCount()
	{
		iCount = 0;
	}

	ClassCount(string _sClassName, int _iCount)
	{
		sClassName = _sClassName; iCount = _iCount;
	}
};

class Node;	//forward declaration

struct link
{
	string sAttributeValue;
	Node * child;

	link(string _sAttributeValue, Node * _child)
	{
		sAttributeValue = _sAttributeValue; child = _child;
	}
};

class Node
{
public:
	string sAttributeName;
	vector < link > child_link;
	vector <ClassCount> class_count;
	ClassCount major_class;
	bool bIsLeaf;

	//public:
	// constructor
	Node()
	{
		bIsLeaf = true;
	}
	void set_class_count()
	{

		for (int i = 0; i < class_atb.size(); i++)
			class_count.push_back(ClassCount(class_atb[i], 0));

	}

	void set_major_class(string _sClassName, int _iCount)
	{
		major_class.sClassName = _sClassName;
		major_class.iCount = _iCount;
	}

};

class B_Tree
{
public:
	B_Tree();



private:
	Node * root;
	Node * current;



};

B_Tree::B_Tree()
{

}

struct Attribute
{
	string sName;
	vector <string> value;	// value[0] stores "numeric"/"categorical"
	bool bUsed;				// we dont consider used attribute in decision tree

	Attribute(string _sName, vector <string> _value, bool _bUsed) { sName = _sName; value = _value; bUsed = _bUsed; }	// constructor for the use of vector.push_back()
};

struct Attribute_Class_Count
{
	string sName;
	vector <int> class_count;

	Attribute_Class_Count(string _sName, int _iN) { sName = _sName; class_count.clear(); class_count.resize(_iN); }
};

struct Split_Info
{
	string sAtbName;
	double fEntropy;

	Split_Info(string _sName, double _fEntropy) { sAtbName = _sName; fEntropy = _fEntropy; }
};

Node * ID3(vector < vector<string> > data, vector <Attribute> data_attribute)
{
	Node * node = new Node;
	node->set_class_count();

	// building decision tree
	// stop condition: check if all class are the same
	if (data.empty())
	{
		node->bIsLeaf = true;
		return node;
		//return NULL;
	}
	string sCheckSame = data[0].back();
	bool bAllSame = true;
	for (int i = 0; i < data.size(); i ++)
	{
		// import total class count to node
		for (int k = 0; k < node->class_count.size(); k ++)
		{
			if (data[i].back() == node->class_count[k].sClassName)
			{
				node->class_count[k].iCount++;
				break;
			}
		}
		
		// check if all class are same
		if (data[i].back() != sCheckSame)	// class attribute is the last column
			bAllSame = false;

	}
	// store majority class first
	int max = 0;
	for (int k = 0; k < node->class_count.size(); k ++)
	{
		if (node->class_count[k].iCount > max)
		{
			max = node->class_count[k].iCount;
			node->set_major_class(node->class_count[k].sClassName, node->class_count[k].iCount);
		}
	}

	if (bAllSame)
	{
		node->bIsLeaf = true;
		return	node;
	}

	// exam all attribute in the dataset
	vector <Split_Info> Entropy;
	node->bIsLeaf = true;
	for (int j = 0; j < data[0].size() - 1; j ++)	// class attribute does not count
	{
		// ignore the attribute used
		if (data_attribute[j].bUsed)
			continue;
		else
			node->bIsLeaf = false;

		// store attribute and class count
		vector <Attribute_Class_Count> ACC;
		// initial ACC
		if (data_attribute[j].value.front() == "numeric")	// if numeric
		{
			continue;
		}
		else
		{
			for (int z = 1; z < data_attribute[j].value.size(); z ++)
				ACC.push_back(Attribute_Class_Count(data_attribute[j].value[z], class_atb.size()));
		}

		for (int i = 0; i < data.size(); i ++)
		{
			// find attribute name
			for (int a = 0; a < ACC.size(); a ++)
			{
				if (data[i][j] == ACC[a].sName)
				{
					// find class
					for (int c = 0; c < class_atb.size(); c ++)
					{
						if (data[i].back() == class_atb[c])
						{
							ACC[a].class_count[c] ++;
							break;
						}
					}
					break;
				}
			}
		}

		// calculate Entropy
		double fEntropy = 0.0;
		double dTotalClassCountPerAtb = 0.0;
		for (int c = 0; c < ACC.size(); c++)
		{
			double fEntropy_part = 0.0;
			// total class count per attribute
			double iTotalClassCount = 0.0;
			for (int ac = 0; ac < ACC[c].class_count.size(); ac++)
				iTotalClassCount += ACC[c].class_count[ac];

			for (int ac = 0; ac < ACC[c].class_count.size(); ac++)
			{
				if (iTotalClassCount == 0.0 || ACC[c].class_count[ac] == 0.0)
					continue;

				fEntropy_part -= (((double)ACC[c].class_count[ac] / iTotalClassCount) * log2(((double)ACC[c].class_count[ac] / iTotalClassCount)));//cout << ACC[c].sName << " " << ACC[c].class_count[ac] << " "<< iTotalClassCount << " "<< fEntropy<<endl;
			}

			fEntropy += iTotalClassCount * fEntropy_part;
			dTotalClassCountPerAtb += iTotalClassCount;
		}

		// save Entropy for each attribute
		Entropy.push_back(Split_Info(data_attribute[j].sName, (fEntropy / dTotalClassCountPerAtb)));
		//cout << Entropy.back().sAtbName << fEntropy << endl;
		ACC.clear();
	}

	// stop when no attribute left
	if (node->bIsLeaf)
		return node;

	// store attribute to node
	int ii, jjj = 0;
	float min = Entropy[0].fEntropy;
	for (ii = 0; ii < Entropy.size(); ii++)
	{
		//float min = Entropy[0].fEntropy;
		if (Entropy[ii].fEntropy < min)
		{
			min = Entropy[ii].fEntropy;
			jjj = ii;
		}
	}
	//cout << Entropy[jjj].sAtbName;
	node->sAttributeName = Entropy[jjj].sAtbName;


	// data reduction
	vector < vector<string> > data_reduction;
	// find the attribute we split on
	for (int j = 0; j < data_attribute.size() - 1; j++)		// class attribute does not count
	{
		if (node->sAttributeName == data_attribute[j].sName)
		{
			// for each value in attribute, we recurrsive it and link it to attribute.child
			for (int av = 1; av < data_attribute[j].value.size(); av++)
			{
				// ini data_reduction
				data_reduction = data;

				// reduction
				for (int i = 0; i < data_reduction.size(); i++)
				{
					if (data_reduction[i][j] != data_attribute[j].value[av])
					{
						// remove row
						data_reduction.erase(data_reduction.begin() + i);
						i--;
					}
				}

				// recursive with the reducted data
				data_attribute[j].bUsed = true;
				node->child_link.push_back(link(data_attribute[j].value[av], ID3(data_reduction, data_attribute)));
			}
			break;
		}
	}

	// return the * node
	return node;
}

void display_tree(Node * n, string _tab)
{
	if (n == NULL)
		return;
	if (n->bIsLeaf)
	{
		out << " : " << n->major_class.sClassName << endl;
		return;
	}

	for (int i = 0; i < n->child_link.size(); i++)
	{
		out << _tab << n->sAttributeName << " = " << n->child_link[i].sAttributeValue;
		if (!(n->child_link[i].child && n->child_link[i].child->bIsLeaf))
			out << endl;

		// recursive child
		display_tree(n->child_link[i].child, (_tab + "|	"));
	}
}

bool test(vector <string> test_data, vector <Attribute> data_attribute, Node * root)
{
	Node * current = root;
	Node * parent = root;
	string predict_class;

	while (!current->bIsLeaf)
	{
		// find data attribute
		for (int i = 0; i < data_attribute.size(); i++)
		{
			if (data_attribute[i].sName == current->sAttributeName)
			{
				// find attribute value
				for (int j = 0; j < current->child_link.size(); j++)
				{
					if (test_data[i] == current->child_link[j].sAttributeValue)
					{
						parent = current;
						current = current->child_link[j].child;
					}
				}
			}
		}
	}

	if (current->major_class.iCount == 0)
		predict_class = parent->major_class.sClassName;
	else
		predict_class = current->major_class.sClassName;

	return (test_data.back() == predict_class) ? true : false;
}

int main()
{
	// input data
	fstream input("nursery-data-15.arff", ios::in);

	fstream test_file("nursery-test.arff", ios::in);

	string sLine;

	// reconizing attribute
	vector <Attribute> data_attribute;
	while (getline(input, sLine) && sLine.find("@data") == string::npos)
	{
		if (sLine.find("@attribute") != string::npos)
		{
			vector <string> Tmp_Vtr = split(sLine, ' ');	//first line is "@attribute"
			data_attribute.push_back(Attribute(Tmp_Vtr[1], split(Tmp_Vtr[2], ','), false));

			// categorical attribute
			if (data_attribute.back().value.front().front() == '{')
			{
				// erase both '{' and '}' in attribute data
				data_attribute.back().value.front().erase(data_attribute.back().value.front().begin());
				data_attribute.back().value.back().erase(data_attribute.back().value.back().end() - 1);

				// value[0] stores "numeric"/"categorical"
				data_attribute.back().value.insert(data_attribute.back().value.begin(), "categorical");
			}
		}
	}

	// class attribute initialize
	for (int i = 1; i < data_attribute.back().value.size(); i++)	// I assume the last attribute is class attribute
		class_atb.push_back(data_attribute.back().value[i]);

	vector < vector<string> > data;
	vector <string> vLine;

	while (input >> sLine)
	{
		vLine.clear();
		vLine = split(sLine, ',');

		// dummy test
		// if (vLine.size())

		data.push_back(vLine);
	}
	// end input data

	Node * root;
	root = ID3(data, data_attribute);

	display_tree(root, "");
	

	// test data
	vector < vector<string> > test_data;

	while (getline(test_file, sLine) && sLine.find("@data") == string::npos);
	while (test_file >> sLine)
	{
		vLine.clear();
		vLine = split(sLine, ',');

		// dummy test
		// if (vLine.size())

		test_data.push_back(vLine);
	}
	// end input test data

	float accuracy = 0.0;
	int TP = 0;
	for (int i = 0; i < test_data.size(); i ++)
		TP += test(test_data[i], data_attribute, root);
	accuracy = ((float)TP / test_data.size()) * 100.0;
	out << accuracy << "%" << endl;

	return 0;
}





