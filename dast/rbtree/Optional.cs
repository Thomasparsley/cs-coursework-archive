public class Optional<T>
{
    private T? value;

    public Optional()
    {
        value = default(T);
    }

    public Optional(T value)
    {
        this.value = value;
    }

    public bool IsNone() => value is null;
    public bool IsSome() => !IsNone();

    public T Get()
    {
        if (value is null)
            throw new System.Exception("Optional.Get() called on empty Optional");

        return value;
    }
}
