public class CircularQueue extends DSAQueue
{
    private int head;
    private int tail;

    public CircularQueue()
    {
        super();
        head = 0;
        tail = 0;
    }

    public CircularQueue(int maxCapacity)
    {
        super(maxCapacity);
        head = 0;
        tail = 0;
    }

    public void enqueue(Object value)
    {
        if (isFull())
        {
            throw new IllegalArgumentException("Queue is full");
        }
        else
        {
            queue[tail] = value;
            tail = (tail+1) % queue.length;
            count += 1;
        }
    }

    public Object dequeue()
    {
        Object bottomVal;

        bottomVal = peek();
        queue[head] = null;
        head  = (head+1) % queue.length;

        count += -1;

        return bottomVal;
    }

    public Object peek()
    {
        Object bottomVal;
        if(isEmpty())
        {
            throw new IllegalArgumentException("Queue is empty");
        }
        else
        {
            bottomVal = queue[head];
        }

        return bottomVal;
    }
}
