struct Pair
{
    String *array;
    int length;

    Pair() = delete;

    Pair(String *array, const int &length)
    {
        this->array = array;
        this->length = length;
    }

    ~Pair()
    {
        delete this->array;
    }

    Pair(const Pair &copy)
    {
        this->length = copy.length;
        this->array = new String[this->length];
        for (int i = 0; i < this->length; i++)
        {
            this->array[i] = copy.array[i];
        }
    }

    Pair(Pair &&move)
    {
        this->length = move.length;
        this->array = move.array;
        move.array = nullptr;
    }
};

Pair split(const String &string, const char &delimiter)
{
    // Calcolare la dimensione dell'array da allocare
    int words = 0;
    int index = -1;

    do
    {
        index = string.indexOf(delimiter, index + 1);
        words++;
    } while (index != -1);

    // Allocazione array
    String *splitted = new String[words];

    for (int i = 0; i < words; i++)
    {
        index = string.indexOf(delimiter);

        if (index == -1)
        {
            splitted[i] = string;
        }
        else
        {
            splitted[i] = string.substring(0, index);
            string.remove(0, index + 1);
        }
    }

    return Pair(splitted, words);
}

void split(const char* data, const char &delimiter)
{
    
}