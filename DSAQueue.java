public abstract class DSAQueue
{
    public static final int DEFAULT_CAPACITY = 100;

    protected Object[] queue;
    protected int count;

    public DSAQueue()
    {
        queue = new Object[DEFAULT_CAPACITY];
        count = 0;      
    }
    
    public DSAQueue(int maxCapacity)
    {
        queue = new Object[maxCapacity];
        count = 0;   
    }

    public int getCount()
    {
        return count;   
    }
    
    public boolean isEmpty()
    {
        return (count==0);
    }

    public boolean isFull()
    {
        return (count==queue.length);
    }

    public abstract void enqueue(Object value);

    public abstract Object dequeue();
    
    public abstract Object peek();
}
