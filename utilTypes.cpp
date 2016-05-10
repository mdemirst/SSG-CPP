#include "utilTypes.h"

//////////////////////////////
//TreeNode Class Definitions//
//////////////////////////////

TreeNode::TreeNode()
{
    label = 0;
    parent = NULL;
    val = 0;
}

TreeNode::TreeNode(int label, double val)
{
    parent = NULL;
    this->label = label;
    this->val = val;
}

void TreeNode::addChild(TreeNode* node)
{
    node->parent = this;
    children.push_back(node);
}

vector<TreeNode*>& TreeNode::getChildren()
{
    return children;
}
