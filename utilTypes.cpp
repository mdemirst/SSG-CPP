#include "utilTypes.h"
#include "utils.h"

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

Dataset::Dataset(string location, int start_idx, int end_idx, int dataset_id)
{
    this->location = location;
    this->start_idx = start_idx;
    this->end_idx = end_idx;
    this->dataset_id = dataset_id;
    files = getFiles(location);
}

