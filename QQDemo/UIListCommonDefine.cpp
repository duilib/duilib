#include "stdafx.h"
#include "UIListCommonDefine.hpp"

namespace DuiLib
{

double CalculateDelay(double state)
{
    return pow(state, 2);
}

void Node::set_parent(Node* parent)
{
	parent_ = parent;
}

Node::Node()
	: parent_(NULL)
{}

Node::Node(NodeData t)
	: data_(t)
	, parent_(NULL)
{}

Node::Node(NodeData t, Node* parent)
: data_ (t)
, parent_ (parent)
{}

Node::~Node() 
{
	for (int i = 0; i < num_children(); ++i)
		delete children_[i]; 
}

NodeData& Node::data()
{
	return data_;
}

int Node::num_children() const
{
	return static_cast<int>(children_.size());
}

Node* Node::child(int i)
{
	return children_[i];
}

Node* Node::parent()
{
	return ( parent_);
}

bool Node::has_children() const
{
	return num_children() > 0;
}

bool Node::folder() const
{
	return data_.folder_;
}

void Node::add_child(Node* child)
{
	child->set_parent(this); 
	children_.push_back(child); 
}


void Node::remove_child(Node* child)
{
	Children::iterator iter = children_.begin();
	for (; iter < children_.end(); ++iter)
	{
		if (*iter == child) 
		{
			children_.erase(iter);
			return;
		}
	}
}

Node* Node::get_last_child()
{
	if (has_children())
	{
		return child(num_children() - 1)->get_last_child();
	}
	return this;
}

}