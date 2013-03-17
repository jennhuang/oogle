#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

#include <queue>
#include <cstdlib>

template <class KeyType, class ValueType>
class MyMap
{
public:
    MyMap()
    {
		m_size = 0;
		m_root = NULL;
    }

    ~MyMap()
    {
		deleteAll(m_root);
    }

    void clear()
    {
		deleteAll(m_root);
		m_size = 0;
		m_root = NULL;
		while(!m_queue.empty())
			m_queue.pop();
    }

    int size() const
    {
        return m_size;
    }

    void associate(const KeyType& key, const ValueType& value)
    {
		insert(m_root, key, value);
    }
	//*******************FIND***********************
	//returns pointer to the value of the given key
	//returns NULL if not found
    const ValueType* find(const KeyType& key) const
    {
        //Node* n = search(m_root, key);
	
		Node* ptr = m_root;
		while (ptr != NULL)
		{
			if (ptr->m_key == key) 
				return &(ptr->m_value);
			else if (ptr->m_key > key)
				ptr = ptr->m_left;
			else
				ptr = ptr->m_right;
		}
		return NULL;
	}
        
    ValueType* find(const KeyType& key)
    {
          // Do not change the implementation of this overload of find
        const MyMap<KeyType,ValueType>* constThis = this;
        return const_cast<ValueType*>(constThis->find(key));
    }

    ValueType* getFirst(KeyType& key)  
    {
        if(m_root == NULL)
			return NULL;
		else
		{
			//place the head node in the queue
			if(m_root->m_left != NULL)
				m_queue.push(m_root->m_left);
			if(m_root->m_right != NULL)
				m_queue.push(m_root->m_right);

			key = m_root->m_key;
			return &(m_root->m_value);  //returns a pointer to the first value
		}
    }

    ValueType* getNext(KeyType& key)
    {
		if(m_queue.empty())
		   return NULL;
		m_cur = m_queue.front();
		m_queue.pop();
		key = m_cur->m_key;   //stores key in the parameter
		if(m_cur->m_left != NULL)
			m_queue.push(m_cur->m_left);
		if(m_cur->m_right != NULL)
			m_queue.push(m_cur->m_right);
	   return &(m_cur->m_value);  //returns a pointer to the next value
    }

private:
    MyMap(const MyMap &other);
    MyMap &operator=(const MyMap &other);
	 struct Node
	{
		KeyType m_key;
		ValueType m_value;
		Node* m_left;
		Node* m_right;
	};
	
	 int m_size;
	Node* m_root;

   void insert(Node*& node, KeyType key, ValueType value)
   {
	   //tree is empty
	   if(node == NULL)
	   {
		    node = new Node;
			node->m_key = key;
			node->m_value = value;
			node->m_left = NULL;
			node->m_right = NULL;
			m_size++; 
			//set the root pointer
			if(m_size == 1)
			{
				m_root = node;
			}
	   }
	 
	   //current node has the same key
	   //update the value
	   if(node->m_key == key)
	   {
		   node->m_value = value;
	   }
	   //current node has bigger key
	   //insert to the left
	   else if(node->m_key > key)
		   insert(node->m_left, key, value);
	   //current node has smaller key
	   //insert to the right
	   else
		   insert(node->m_right, key, value);
   }
   ////////////////////////////////////////////////
  
   //make a queue for my level order traversal
   std::queue<Node*> m_queue;

   //have a current node pointer
   Node* m_cur;
 
   void deleteAll(Node* node)
   {
	   if(node == NULL)
		   return;
	   deleteAll(node->m_left);
	   deleteAll(node->m_right);
	   delete node;
   }

   Node* search(Node* node, KeyType key) const
   {
	   if(node == NULL)
		   return NULL;
	   else if(node->m_key == key)
		   return node;
	   else if(node->m_key < key)
		   search(node->m_right, key);
	   else if(node->m_key > key)
		   search(node->m_left, key);
   }
};

#endif // MYMAP_INCLUDED