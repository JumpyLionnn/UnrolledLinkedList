# Unrolled Linked List
here is an implementation of an unrolled linked list in c++.
inserting, removing and iterating is supported.
its not feature complete.
random element access is not supported.
https://en.wikipedia.org/wiki/Unrolled_linked_list

there are 2 kinds of this list:
one for a static list faster but less features
and the other one a more dynamic version where all of the elements are guaranteed to stay in the same place in memory without leaving unfixable memory holes.

the performence of both of those are better or close to a standard list and a standard vector in inserting removing and iterating most of the time.

this implementation uses c++20 and was tested only on windows with MSVC although there is nothing platform/compiler specific.

an implementation of a bitset has been implemented because the standard version of a bitset doesnt support a few needed features for this to work.

a file is linked there (main.cpp) to test and benchmark this datastructure compared to the standard ones