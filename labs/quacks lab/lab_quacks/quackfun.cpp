/**
 * @file quackfun.cpp
 * This is where you will implement the required functions for the
 *  stacks and queues portion of the lab.
 */

namespace QuackFun
{

    /**
     * Sums items in a stack.
     * @param s A stack holding values to sum.
     * @return The sum of all the elements in the stack, leaving the original
     *  stack in the same state (unchanged).
     *
     * @note You may modify the stack as long as you restore it to its original
     *  values.
     * @note You may use only two local variables of type T in your function.
     *  Note that this function is templatized on the stack's type, so stacks of
     *  objects overloading the + operator can be summed.
     * @note We are using the Standard Template Library (STL) stack in this
     *  problem. Its pop function works a bit differently from the stack we
     *  built. Try searching for "stl stack" to learn how to use it.
     * @hint Think recursively!
     */
    template <typename T>
    T sum(stack<T> &s)
    {
        // Your code here
        if (s.empty()) return T();

        T top = s.top();
        s.pop();
        T total = top + sum(s);
        s.push(top);
        return total;
    }

    /**
     * Reverses even sized blocks of items in the queue. Blocks start at size
     * one and increase for each subsequent block.
     * @param q A queue of items to be scrambled
     *
     * @note Any "leftover" numbers should be handled as if their block was
     *  complete.
     * @note We are using the Standard Template Library (STL) queue in this
     *  problem. Its pop function works a bit differently from the stack we
     *  built. Try searching for "stl stack" to learn how to use it.
     * @hint You'll want to make a local stack variable.
     */
    template <typename T>
    void scramble(queue<T> &q)
    {
        stack<T> s;

        // Your code here
        int counter = q.size();

        for (int blocksize = 1; counter != 0; blocksize++) {
            // odd case
            if (blocksize % 2) {
                for (int i = 0; i < blocksize && counter != 0; i++) {
                    q.push(q.front());
                    q.pop();
                    counter--;
                }
            // even case
            } else {
                for (int i = 0; i < blocksize && counter != 0; i++) {
                    s.push(q.front());
                    q.pop();
                    counter--;
                }

                while (!s.empty()) {
                    q.push(s.top());
                    s.pop();
                }
            }
        }

        // shorter alternative solution
        // int counter = q.size();

        // for (int blocksize = 1; counter; blocksize += 2)
        // {
        //     // odd case: move first blocksize elements to back of queue
        //     for (int i = 0; (i < blocksize) && counter; q.push(q.front()), i++, counter--, q.pop());

        //     // even case: move first blocksize elements to stack
        //     for (int i = 0; (i < blocksize + 1) && counter; s.push(q.front()), i++, counter--, q.pop());

        //     // and push them to back of queue
        //     while (!s.empty())
        //     {
        //         q.push(s.top());
        //         s.pop();
        //     }
        // }
    }

    /**
     * @return true if the parameter stack and queue contain only elements of
     *  exactly the same values in exactly the same order; false, otherwise.
     *
     * @note You may assume the stack and queue contain the same number of items!
     * @note There are restrictions for writing this function.
     * - Your function may not use any loops
     * - In your function you may only declare ONE local boolean variable to use in
     *   your return statement, and you may only declare TWO local variables of
     *   parametrized type T to use however you wish.
     * - No other local variables can be used.
     * - After execution of verifySame, the stack and queue must be unchanged. Be
     *   sure to comment your code VERY well.
     */
    template <typename T>
    bool verifySame(stack<T> &s, queue<T> &q)
    {
        if (s.empty()) return true;

        // keep the top of the stack to compare with back of queue
        T top = s.top();
        s.pop();

        // go to end first, because stack top is actually queue back
        // this effectively checks the stack and queue from the bottom/front
        // the last recursive call returns true (empty stack),
        // then the top (5) is compared with the queue front (5).
        // etc etc
        bool retval = verifySame(s, q) && (top == q.front());

        // restore the stack, now that we're done comparing the top element
        s.push(top);

        // this makes the second element the new front,
        // so for example after comparing 5, 4 becomes the new front
        q.push(q.front());
        q.pop();

        return retval;
    }
}