/********************************************************
File name: treeNode.h
This file contains the definition of the AST.
We can create a node using the function new_node.
********************************************************/


#ifndef TREENODE_H
#define TREENODE_H

#include "header.h"

typedef struct TreeNode{
	char *data;
	char *type;
	int line, childNum;
	struct TreeNode** children;
} TreeNode;

TreeNode* root;

TreeNode* new_node(int line, const char* type, const char *data, int childNum, ...){
	va_list ap;
	TreeNode* current = (TreeNode*)malloc(sizeof(TreeNode));
	current -> data = strdup(data);
	current -> type = strdup(type);
	current -> line = line;
	current -> childNum = childNum;
	if (childNum > 0){
		current -> children = (TreeNode**)malloc(sizeof(TreeNode*) * childNum);
		va_start(ap, childNum);
		int i = 0;
		while (i < childNum){
			current -> children[i] = va_arg(ap, TreeNode*);
			i++;
		}
		va_end(ap);
	}
	return current;
}


void print_root(TreeNode* root){
	printf("%s\n", root -> data);
}

void print_node(TreeNode* node, int height){
	int i = height * 2;
	while (i > 0){
		printf("_");
		i--;
	}
	printf("|%s\n", node -> data);
	int num = node -> childNum;
	for(i = 0; i <num; ++i){
		print_node(node -> children[i], height + 1);
	}
}

void print_tree(TreeNode* root){
	print_root(root);
	int i;
	int childNum = root -> childNum;
	for (i = 0; i < childNum; ++i){
		print_node(root -> children[i], 1);
	}
}

#endif