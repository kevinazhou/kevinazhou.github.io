  #include <algorithm>
  #include <vector>



  template <class T>
  class MinHeap {
     public:
       MinHeap (int d);
       /* Constructor that builds a d-ary Min Heap
          This should work for any d >= 2,
          but doesn't have to do anything for smaller d.*/

       ~MinHeap ();

       void add (T item, int priority);
         /* adds the item to the heap, with the given priority. */

       const T & peek () const;
         /* returns the element with smallest priority.  
            Break ties however you wish.  
            Throws an exception if the heap is empty. */

       void remove ();
         /* removes the element with smallest priority.
            Break ties however you wish.
            Throws an exception if the heap is empty. */

       bool isEmpty ();
         /* returns true iff there are no elements on the heap. */

   private:
      // whatever you need to naturally store things.
      // You may also add helper functions here.
      std::vector<std::pair<int, T> > heap;
      int num;//num-ary tree
      void trickleUp(size_t loc, std::pair<int, T> toAdd);
      void heapify(size_t idx);
  };

  template <class T>
  MinHeap<T>::MinHeap(int d)
  {
    num=d;
  }

  template <class T>
  MinHeap<T>::~MinHeap() {}

  template <class T>
  void MinHeap<T>::add(T item, int priority)
  {
    std::pair<int, T> toAdd;
    toAdd.first = priority;
    toAdd.second = item;

    heap.push_back(toAdd);
    size_t addingIndex = heap.size()-1;
    trickleUp(addingIndex, toAdd);

  }

  template <class T>
  void MinHeap<T>::trickleUp(size_t loc, std::pair<int, T> toAdd)
  {
    while(loc>0)
    {
      size_t parentIndex = (loc-1)/num;
      if(parentIndex >= 0 && heap[parentIndex].first > toAdd.first)
      {//swap up if out of order
        std::iter_swap(heap.begin()+parentIndex, heap.begin()+loc);
        loc = parentIndex;
      }
      else
      {
        break;
      }
    }
  }

  template <class T>
  const T & MinHeap<T>::peek () const
  {
    if(heap.size()==0)
    {
      throw std::runtime_error("Empty");
    }

    return heap[0].second;
  }

  template <class T>
  void MinHeap<T>::remove ()
  {
    if(heap.size()==0)
    {
      throw std::runtime_error("Empty");
    }

    heap[0] = heap[heap.size()-1];//switch first element with last, then heapify
    heap.pop_back();

    heapify(0);

  }

  template <class T>
  void MinHeap<T>::heapify(size_t idx)
  {
    if((idx*num)+1>=heap.size())
    {
      return;//when hitting a leaf node
    }
    int lowestChildVal = heap[(num*idx)+1].first;
    int lowestChildInd = (num*idx)+1;
    for(int i=2; i<=num; i++)//checking all the childs
    {
      if(heap.size()>(num*idx)+i)//if child exists
      {
        std::pair<int, T> child = heap[(num*idx)+i];
        if(lowestChildVal>child.first)//to find lowest child
        {
          lowestChildVal = child.first;
          lowestChildInd = (num*idx)+i;
        }
      }
    }

    if(lowestChildVal<heap[idx].first)//swap and recurse if children lower than current
    {
      std::iter_swap(heap.begin()+idx, heap.begin()+lowestChildInd);
      heapify(lowestChildInd);
    }
 
  }

  template <class T>
  bool MinHeap<T>::isEmpty ()
  {
    if(heap.size()==0)
    {
      return true;
    }
    return false;
  }
