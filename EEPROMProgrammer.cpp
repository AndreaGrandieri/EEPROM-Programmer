// AGGIUNGERE DELAY DI SICUREZZA PER LA LATENZA DEL CHIP EEPROM
/*
   Grandieri Andrea

   EEPROM Programmer w/Mega2560
*/

#define DEFAULT_BAUD_RATE 9600

/*#define ENTIRETY_ADDRESSES int address = 0; address < ADDRESSES; address++*/
/*#define ENTIRETY_ADDRESSING_PIN int i = 0; i < ADDRESSING_PIN; i++*/
/*#define ENTIRETY_SEGMENT int offset = 0; offset < SEGMENT_DEPTH; offset++*/
/*#define ENTIRETY_ADDRESSES_AS_SEGMENTS int baseAddress = 0; baseAddress < ADDRESSES; baseAddress += SEGMENT_DEPTH*/
/*#define ENTIRETY_READONLY_SEGMENTS int i = 0; i < READONLY_SEGMENTS; i++*/
/*#define ENTIRETY_DATA_EXCHANGING_PIN int i = 0; i < DATA_EXCHANGING_PIN; i++*/
/*#define ADDRESS_VALIDITY_CHECK address >= HIGHEST_ADDRESS &&address <= LOWEST_ADDRESS*/
/*#define BASE_ADDRESS_VALIDITY_CHECK baseAddress >= HIGHEST_ADDRESS &&baseAddress <= LOWEST_ADDRESS*/
/*#define SEGMENT_VALIDITY_CHECK ((baseAddress % SEGMENT_DEPTH) == 0)*/
/*#define DATA_VALIDITY_CHECK data >= LOWEST_VALID_DATA_VALUE &&data <= HIGHEST_VALID_DATA_VALUE*/

/*USER FILL-IN*/
/*
L'utente utilizzatore di tale codice dovrebbe specificare alcuni valori
relativi al chip eeprom in uso. Il seguente codice non è specializzato
per alcun chip eeprom. Non si assumono responsabilità per eventuali
danni al chip eeprom causati da erronea specificazione dei valori.
*/

class EEPROMManager
{
    /*USER FILL-IN*/
private:
    // Timing
    // Lettura
    // Il tempo che trascorre tra l'impulso di lettura e l'effettiva presenza
    // di dati campionabili sui pin di dataIO
    static constexpr int TIME_WAIT_READ = 1; /*microsecondi*/

    // Il tempo di recupero dopo una lettura
    static constexpr int TIME_RECOVERY_FROM_READ = 0; /*millisecondi*/

    // Scrittura
    // Il tempo di hold per far si di generare un segnale di inizio
    // scrittura valido per il chip
    static constexpr int TIME_HOLD_WRITE_SIGNAL = 1; /*microsecondi*/

    // Il tempo che impiega il chip fisicamente a scrivere
    // i dati nella memoria
    static constexpr int TIME_WRITE = 1; /*millisecondi*/

    // Il tempo di recupero dopo una scrittura
    static constexpr int TIME_RECOVERY_FROM_WRITE = 0; /*millisecondi*/

    // L'imprecisione, espressa in millisecondi/microsecondi, del chip
    static constexpr int IMPRECISION = 200; /*microsecondi*/

    // Il numero di pin di addressing (scelta dell'indirizzo)
    static constexpr int ADDRESSING_PIN = 11;

    // Il numero di pin data (per I/O dati)
    static constexpr int DATA_EXCHANGING_PIN = 8;

    // Profondità dei segmenti di memoria
    static constexpr int SEGMENT_DEPTH = 16;

    // Flag indicante l'uso o non di segmenti readonly (sola lettura)
    static constexpr bool READONLY_SEGMENTS_ALLOWED = false;

    // Il numero di segmenti readonly (sola lettura) usati.
    // Se non usati, valore '0'
    static constexpr int READONLY_SEGMENTS = 0;

    // Lista contenente un indirizzo base per ogni segmento readonly usato.
    // Se non usati, lista vuota
#define LIST_READONLY_SEGMENTS \
    {                          \
    }

    /*PADDING*/
    // Il padding permette di visualizzare un output "più elegante"
    // Add more padding checkers if needed following this rule:
    // PADDING_CHECK_n = 16^n
private:
    static constexpr int PADDING_CHECK_ONE = 16;  /*16^1*/
    static constexpr int PADDING_CHECK_TWO = 256; /*16^2*/

    /*DO NOT MODIFY*/
private:
    // Il numero di indirizzi totali (non tutti raggiungibili)
    static constexpr int ADDRESSES = pow(2, ADDRESSING_PIN); /*DEDUCED*/

    // L'indirizzo "più basso" (con valore algebricamente più alto) raggiungibile
    static constexpr int LOWEST_ADDRESS = ADDRESSES - 1; /*DEDUCED*/

    // L'indirizzo "più alto" (con valore algebricamente più basso) raggiungibile
    static constexpr int HIGHEST_ADDRESS = 0;

    // Il numero di valori validi realizzabili con il numero
    // di bit del chip (non tutti utilizzabili)
    static constexpr int VALID_DATA_VALUES = pow(2, DATA_EXCHANGING_PIN); /*DEDUCED*/

    // Il valore algebricamente più basso utilizzabile
    static constexpr int LOWEST_VALID_DATA_VALUE = 0;

    // Il valore algebricamente più alto utilizzabile
    static constexpr int HIGHEST_VALID_DATA_VALUE = VALID_DATA_VALUES - 1; /*DEDUCED*/

    /*DO NOT MODIFY*/
private:
    // Pin arduino per l'output enable
    const int outputEnable;

    // Pin arduino per il write enable
    const int writeEnable;

    // [Array] Pin arduino per la scelta dell'indirizzo
    const int *addresses;

    // [Array] Pin arduino per I/O dati
    const int *dataIO;

    // [Array] Segmenti readonly (sola lettura)
    const int *readonlySegments;

    // Flag indicante lo stato di inizializzazione di "this"
    bool hasBeenInit;

public:
    // Enumerazione per indicare la modalità di interpretazione
    // dei dati letti
    enum class ReadMode
    {
        _DEC,
        _HEX,
        _BIN,
        _OCT,
        ALL
    };

    // Enumerazione per indicare lo stato dei Pin I/O
    // Stato: INPUT
    // Stato: OUTPUT
    enum class DataIOState
    {
        _INPUT,
        _OUTPUT
    };

    // Enumerazione per indicare la modalità di clear del chip
    enum class ClearMode
    {
        LOWEST_VALUE_FILL,
        HIGHEST_VALUE_FILL
    };

private:
    enum class ResultStringBuilderSpecification
    {
        INCLUDE_HEADER_INFO,
        NOT_INCLUDE_HEADER_INFO
    };

    // Enumerazione per indicare la modalità di applicazione
    // del padding
    enum class FormatTarget
    {
        HEADER,
        NOT_HEADER
    };

public:
    // Costruttore di default
    EEPROMManager() = delete;

    // Costruttore copia
    EEPROMManager(const EEPROMManager &copy)
        : outputEnable(copy.outputEnable), writeEnable(copy.writeEnable),
          addresses(new int[ADDRESSING_PIN]), dataIO(new int[DATA_EXCHANGING_PIN]),
          hasBeenInit(false)
    {
        memcpy(this->addresses, copy.addresses, sizeof(int) * ADDRESSING_PIN);
        memcpy(this->dataIO, copy.dataIO, sizeof(int) * DATA_EXCHANGING_PIN);

        if (READONLY_SEGMENTS_ALLOWED)
        {
            this->readonlySegments = new int[READONLY_SEGMENTS];
            memcpy(this->readonlySegments, copy.readonlySegments, sizeof(int) * READONLY_SEGMENTS);
        }
        else
        {
            this->readonlySegments = nullptr;
        }
    }

    // Costruttore movimento
    EEPROMManager(EEPROMManager &&move)
        : outputEnable(move.outputEnable), writeEnable(move.writeEnable),
          addresses(move.addresses), dataIO(move.addresses), readonlySegments(move.readonlySegments),
          hasBeenInit(false)
    {
        move.addresses = nullptr;
        move.dataIO = nullptr;
        move.readonlySegments = nullptr;
    }

    // Operatore assegnamento copia
    void operator=(const EEPROMManager &copy)
    {
        this->outputEnable = copy.outputEnable;
        this->writeEnable = copy.writeEnable;
        delete this->addresses;
        this->addresses = new int[ADDRESSING_PIN];
        delete this->dataIO;
        this->dataIO = new int[DATA_EXCHANGING_PIN];
        delete this->readonlySegments;

        memcpy(this->addresses, copy.addresses, sizeof(int) * ADDRESSING_PIN);
        memcpy(this->dataIO, copy.dataIO, sizeof(int) * DATA_EXCHANGING_PIN);

        if (READONLY_SEGMENTS_ALLOWED)
        {
            this->readonlySegments = new int[READONLY_SEGMENTS];
            memcpy(this->readonlySegments, copy.readonlySegments, sizeof(int) * READONLY_SEGMENTS);
        }
        else
        {
            this->readonlySegments = nullptr;
        }

        this->hasBeenInit = false;
    }

    // Operatore assegnamento movimento
    void operator=(EEPROMManager &&move)
    {
        this->outputEnable = move.outputEnable;
        this->writeEnable = move.writeEnable;
        delete this->addresses;
        this->addresses = move.addresses;
        move.addresses = nullptr;
        delete this->dataIO;
        this->dataIO = move.dataIO;
        move.dataIO = nullptr;
        delete this->readonlySegments;
        this->readonlySegments = move.readonlySegments;
        move.readonlySegments = nullptr;
        this->hasBeenInit = false;
    }

    // Costruttore parametrico
    EEPROMManager(const int &outputEnable, const int &writeEnable, const int &a0, const int &a1,
                  const int &a2, const int &a3, const int &a4, const int &a5, const int &a6, const int &a7,
                  const int &a8, const int &a9, const int &a10, const int &io0, const int &io1, const int &io2,
                  const int &io3, const int &io4, const int &io5, const int &io6, const int &io7)
        : outputEnable(outputEnable), writeEnable(writeEnable),
          addresses(new int[ADDRESSING_PIN]{
              a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10}),
          dataIO(new int[DATA_EXCHANGING_PIN]{io0, io1, io2, io3, io4, io5, io6, io7}),
          hasBeenInit(false)
    {
        if (READONLY_SEGMENTS_ALLOWED)
        {
            this->readonlySegments = new int[READONLY_SEGMENTS] LIST_READONLY_SEGMENTS;
        }
        else
        {
            this->readonlySegments = nullptr;
        }
    }

    // Distruttore
    ~EEPROMManager()
    {
        delete[] this->addresses;
        delete[] this->dataIO;
        delete[] this->readonlySegments;
    }

    // Metodo di inizializzazione di "this"
    void init()
    {
        // Se l'inizializzazione è già avvenuta non la ripeto
        if (!this->hasBeenInit)
        {
            // Disattivo i segnali di lettura e scrittura
            this->setOutputEnable(LOW);
            this->setWriteEnable(LOW);

            // Imposto i pin Arduino "outputEnable" e "writeEnable" su output
            // da Arduino verso il chip (input al chip)
            pinMode(this->outputEnable, OUTPUT);
            pinMode(this->writeEnable, OUTPUT);

            // Imposto l'indirizzo "più alto" '0x000' come indirizzo di default.
            // Questo mi permette di ottenere uno stato ben determinato.
            // Questo per evitare la presenza di stati indeterminati
            for (int i = 0; i < ADDRESSING_PIN; i++)
            {
                digitalWrite(this->addresses[i], LOW);
            }

            // Imposto tutti i pin Arduino "addresses" su output
            // da Arduino verso il chip (input al chip)
            for (int i = 0; i < ADDRESSING_PIN; i++)
            {
                pinMode(this->addresses[i], OUTPUT);
            }

            // Avviso che l'inizializzazione è stata completata
            this->hasBeenInit = true;
        }
    }

    // Metodo per effettuare la lettura di un singolo indirizzo di memoria
    void readAddress(const int &address, const EEPROMManager::ReadMode &mode)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            String result;
            double sampleResult;

            // Controllo la validità dell'indirizzo fornito come parametro
            if (address >= HIGHEST_ADDRESS && address <= LOWEST_ADDRESS)
            {
                // Indirizzo valido
                // Imposto i pin I/O su input. Questo significa che i dati sono in uscita dal chip
                // (output dal chip) e in entrata in Arduino (input in Arduino)
                this->setDataIO(EEPROMManager::DataIOState::_INPUT);

                // Imposto l'indirizzo di memoria da leggere
                this->setAddress(address);

                // LETTURA
                // Abilito l'output da parte del chip
                this->setOutputEnable(HIGH);

                // Attendo il valore di TIME_WAIT_READ
                delayMicroseconds(TIME_WAIT_READ);
                // Tengo conto dell'imprecisione del chip
                delayMicroseconds(IMPRECISION);

                // Campiono
                sampleResult = this->sample();

                // Disabilito l'output da parte del chip
                this->setOutputEnable(LOW);

                // Costruisco il risultato
                // Il metodo 'resultStringBuilder' mi permette di formattare stilisticamente l'output in un formato
                // semplice da interpretare ed elegante
                result = this->resultStringBuilder(address, this->sample(), mode, EEPROMManager::ResultStringBuilderSpecification::INCLUDE_HEADER_INFO);

                // Attendo il valore di TIME_RECOVERY_FROM_READ
                delay(TIME_RECOVERY_FROM_READ);
                // Tengo conto dell'imprecisione del chip
                delayMicroseconds(IMPRECISION);
            }
            else
            {
                // Indirizzo non valido
                result = "Invalid address. No data read.";
            }

            // Visualizzo a schermo
            Serial.println(result);
        }
    }

    // Metodo per effettuare la lettura di un intero segmento di memoria
    void readSegment(const int &baseAddress)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            String result;
            double sampleResult;

            // Controllo la validità dell'indirizzo di base fornito come parametro
            if (baseAddress >= HIGHEST_ADDRESS && baseAddress <= LOWEST_ADDRESS && ((baseAddress % SEGMENT_DEPTH) == 0))
            {
                // Indirizzo valido
                // Costruisco la prima parte del risultato
                // Il metodo 'format' fa riferimento alle informazioni di padding per
                // un output "più elegante"
                result = "0x" + format(baseAddress, EEPROMManager::FormatTarget::HEADER) + String(baseAddress, HEX) + ": ";

                // Imposto i pin I/O su input. Questo significa che i dati sono in uscita dal chip
                // (output dal chip) e in entrata in Arduino (input in Arduino)
                this->setDataIO(EEPROMManager::DataIOState::_INPUT);

                // Continuo per tutta l'estensione del segmento
                for (int offset = 0; offset < SEGMENT_DEPTH; offset++)
                {
                    // Imposto l'indirizzo di memoria da leggere
                    this->setAddress(baseAddress + offset);

                    // LETTURA
                    // Abilito l'output da parte del chip
                    this->setOutputEnable(HIGH);

                    // Attendo il valore di TIME_WAIT_READ
                    delayMicroseconds(TIME_WAIT_READ);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);

                    // Campiono
                    sampleResult = this->sample();

                    // Disabilito l'output da parte del chip
                    this->setOutputEnable(LOW);

                    // Costruisco il risultato
                    // I metodi 'format' e 'resultStringBuilder' mi permettono di formattare stilisticamente l'output in un formato
                    // semplice da interpretare ed elegante
                    result += format(sampleResult, EEPROMManager::FormatTarget::NOT_HEADER);
                    result += this->resultStringBuilder(baseAddress + offset, sampleResult, EEPROMManager::ReadMode::_HEX, EEPROMManager::ResultStringBuilderSpecification::NOT_INCLUDE_HEADER_INFO) + " ";

                    // Attendo il valore di TIME_RECOVERY_FROM_READ
                    delay(TIME_RECOVERY_FROM_READ);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);
                }
            }
            else
            {
                // Segmento non valido
                result = "Invalid segment or address. No data read.";
            }

            // Visualizzo a schermo
            Serial.println(result);
        }
    }

    // Metodo per leggere tutta la memoria del chip
    void readAll()
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            // Scorro tutti gli indirizzi di memoria del chip,
            // trattandoli come segmenti
            for (int baseAddress = 0; baseAddress < ADDRESSES; baseAddress += SEGMENT_DEPTH)
            {
                // Richiamo il metodo per leggere un intero segmento
                this->readSegment(baseAddress);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    // Metodo per effettuare la scrittura di un singolo indirizzo di memoria
    void writeAddress(const int &address, const int &data)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            String result;

            // Controllo la validità dell'indirizzo fornito come parametro
            // e del dato da scrivere
            if (address >= HIGHEST_ADDRESS && address <= LOWEST_ADDRESS && data >= LOWEST_VALID_DATA_VALUE && data <= HIGHEST_VALID_DATA_VALUE)
            {
                // Effettuo la scrittura solo e solamente
                // se l'indirizzo specificato non è readonly (sola lettura)
                if (!this->isReadonly(address))
                {
                    // Imposto i pin I/O su output. Questo significa che i dati sono in uscita da Arduino
                    // (output da Arduino) e in entrata nel chip (input nel chip)
                    this->setDataIO(EEPROMManager::DataIOState::_OUTPUT);

                    // Imposto l'indirizzo di memoria da scrivere
                    this->setAddress(address);

                    // Effettuo l'output del dato da scrivere sui pin I/O
                    // Il dato non verrà comunque ancora scritto
                    this->put(data);

                    // SCRITTURA
                    // Abilito l'input da parte del chip
                    this->setWriteEnable(HIGH);

                    // Attendo il valore di TIME_HOLD_WRITE_SIGNAL
                    delayMicroseconds(TIME_HOLD_WRITE_SIGNAL);
                    // Valore al limite superiore del range, dunque non tengo conto
                    // dell'imprecisione
                    /*
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);
                    */

                    // Disabilito l'input da parte del chip
                    this->setWriteEnable(LOW);

                    // Attengo che avvenga la scrittura
                    // Attendo il valore di TIME_WRITE
                    delay(TIME_WRITE);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);

                    // Costruisco il risultato (stato della operazione di scrittura)
                    result = "Address: 0x" + String(address, HEX) + " written.";

                    // Attendo il valore di TIME_RECOVERY_FROM_WRITE
                    delay(TIME_RECOVERY_FROM_WRITE);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);
                }
                else
                {
                    // Indirizzo readonly (sola lettura). Non può essere scritto.
                    result = "Address: 0x" + String(address, HEX) + " is readonly. No data written.";
                }
            }
            else
            {
                // Indirizzo o dato non valido
                result = "Invalid address or data. No data written.";
            }

            // Visualizzo a schermo
            Serial.println(result);
        }
    }

    // Metodo per effettuare la scrittura di un intero segmento di memoria
    void writeSegment(const int &baseAddress, const int &data)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            String result;

            // Controllo la validità dell'indirizzo di base fornito come parametro e
            // del dato da scrivere
            if (baseAddress >= HIGHEST_ADDRESS && baseAddress <= LOWEST_ADDRESS && data >= LOWEST_VALID_DATA_VALUE && data <= HIGHEST_VALID_DATA_VALUE && ((baseAddress % SEGMENT_DEPTH) == 0))
            {
                // Effettuo la scrittura solo e solamente
                // se il segmento specificato non è readonly (sola lettura)
                if (!this->isReadonly(baseAddress))
                {
                    // Imposto i pin I/O su output. Questo significa che i dati sono in uscita da Arduino
                    // (output da Arduino) e in entrata nel chip (input nel chip)
                    this->setDataIO(EEPROMManager::DataIOState::_OUTPUT);

                    // Continuo per tutta l'estensione del segmento
                    for (int offset = 0; offset < SEGMENT_DEPTH; offset++)
                    {
                        // Imposto l'indirizzo di memoria da scrivere
                        this->setAddress(baseAddress + offset);

                        // Effettuo l'output del dato da scrivere sui pin I/O
                        // Il dato non verrà comunque ancora scritto
                        this->put(data);

                        // SCRITTURA
                        // Abilito l'input da parte del chip
                        this->setWriteEnable(HIGH);

                        // Attendo il valore di TIME_HOLD_WRITE_SIGNAL
                        delayMicroseconds(TIME_HOLD_WRITE_SIGNAL);
                        // Valore al limite superiore del range, dunque non tengo conto
                        // dell'imprecisione
                        /*
                        // Tengo conto dell'imprecisione del chip
                        delayMicroseconds(IMPRECISION);
                        */

                        // Disabilito l'input da parte del chip
                        this->setWriteEnable(LOW);

                        // Attengo che avvenga la scrittura
                        // Attendo il valore di TIME_WRITE
                        delay(TIME_WRITE);
                        // Tengo conto dell'imprecisione del chip
                        delayMicroseconds(IMPRECISION);

                        // Attendo il valore di TIME_RECOVERY_FROM_WRITE
                        delay(TIME_RECOVERY_FROM_WRITE);
                        // Tengo conto dell'imprecisione del chip
                        delayMicroseconds(IMPRECISION);
                    }

                    // Costruisco il risultato (stato della operazione di scrittura)
                    result = "Segment: 0x" + String(baseAddress, HEX) + " written.";
                }
                else
                {
                    // Segmento readonly (sola lettura). Non può essere scritto.
                    result = "Segment: 0x" + String(baseAddress, HEX) + " is readonly. No data written.";
                }
            }
            else
            {
                // Segmento o dato non valido
                result = "Invalid segment or address or data. No data written.";
            }

            // Visualizzo a schermo
            Serial.println(result);
        }
    }

    // Metodo per scrivere tutta la memoria del chip
    void writeAll(const int &data)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            // Controllo la validità del dato da scrivere
            if (data >= LOWEST_VALID_DATA_VALUE && data <= HIGHEST_VALID_DATA_VALUE)
            {
                // Scorro tutti gli indirizzi di memoria del chip,
                // trattandoli come segmenti
                for (int baseAddress = 0; baseAddress < ADDRESSES; baseAddress += SEGMENT_DEPTH)
                {
                    // Richiamo il metodo per scrivere un intero segmento
                    this->writeSegment(baseAddress, data);
                }
            }
            else
            {
                // Dato non valido
                Serial.println("Invalid data. No data written.");
            }
        }
    }

    // Metodo per pulire tutta la memoria del chip (formattazione)
    void clear(const EEPROMManager::ClearMode &mode)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            // Due diverse modalità di clear:
            // LOWEST_VALUE_FILL: tutta la memoria viene "riempita"
            // del valore '0x00'
            // HIGHEST_VALUE_FILL: tutta la memoria viene "riempita"
            // del valore '0xff'

            // Come singola operazione richiamo il metodo per scrivere
            // tutta la memoria del chip
            // -> I segmenti readonly (sola lettura) non verranno interessati
            // da questa procedura
            switch (mode)
            {
            case EEPROMManager::ClearMode::LOWEST_VALUE_FILL:
                this->writeAll(0x00);
                break;

            case EEPROMManager::ClearMode::HIGHEST_VALUE_FILL:
                this->writeAll(0xff);
                break;
            }
        }
    }

    // Metodo per pulire tutta la memoria del chip (formattazione)
    void hardClear(const EEPROMManager::ClearMode &mode)
    {
        // Effettuo operazioni solo se è già stata effettuata
        // l'inizializzazione
        if (this->hasBeenInit)
        {
            // Dato da scrivere
            int data;

            // Due diverse modalità di clear:
            // LOWEST_VALUE_FILL: tutta la memoria viene "riempita"
            // del valore '0x00'
            // HIGHEST_VALUE_FILL: tutta la memoria viene "riempita"
            // del valore '0xff'

            // -> I segmenti readonly (sola lettura) verranno interessati
            // da questa procedura
            switch (mode)
            {
            case EEPROMManager::ClearMode::LOWEST_VALUE_FILL:
                data = 0x00;
                break;

            case EEPROMManager::ClearMode::HIGHEST_VALUE_FILL:
                data = 0xff;
                break;
            }

            // Imposto i pin I/O su output. Questo significa che i dati sono in uscita da Arduino
            // (output da Arduino) e in entrata nel chip (input nel chip)
            this->setDataIO(EEPROMManager::DataIOState::_OUTPUT);

            // Scorro tutti gli indirizzi di memoria del chip,
            // trattandoli come segmenti
            for (int baseAddress = 0; baseAddress < ADDRESSES; baseAddress += SEGMENT_DEPTH)
            {
                // Continuo per tutta l'estensione del segmento
                for (int offset = 0; offset < SEGMENT_DEPTH; offset++)
                {
                    // Imposto l'indirizzo di memoria da scrivere
                    this->setAddress(baseAddress + offset);

                    // Effettuo l'output del dato da scrivere sui pin I/O
                    // Il dato non verrà comunque ancora scritto
                    this->put(0x00);

                    // SCRITTURA
                    // Abilito l'input da parte del chip
                    this->setWriteEnable(HIGH);

                    // Attendo il valore di TIME_HOLD_WRITE_SIGNAL
                    delayMicroseconds(TIME_HOLD_WRITE_SIGNAL);
                    // Valore al limite superiore del range, dunque non tengo conto
                    // dell'imprecisione
                    /*
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);
                    */

                    // Disabilito l'input da parte del chip
                    this->setWriteEnable(LOW);

                    // Attengo che avvenga la scrittura
                    // Attendo il valore di TIME_WRITE
                    delay(TIME_WRITE);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);

                    // Attendo il valore di TIME_RECOVERY_FROM_WRITE
                    delay(TIME_RECOVERY_FROM_WRITE);
                    // Tengo conto dell'imprecisione del chip
                    delayMicroseconds(IMPRECISION);
                }
            }
        }
    }

private:
    // Seleziona l'indirizzo specificato
    void setAddress(int address)
    {
        // Per impostare l'indirizzo specificato,
        // esso viene "scomposto" nei bit che lo compongono
        // ed ogni bit, uno alla volta, viene trasmesso al chip
        // attraverso i pin Arduino di addressing (array addresses).
        // Per effettuare la "scomposizione" viene utilizzato l'operatore
        // "and bitwise [&]".
        // Per avanzare "un bit alla volta" viene utilizzato l'operatore
        // "right shift [>>]"
        for (int i = 0; i < ADDRESSING_PIN; i++)
        {
            digitalWrite(this->addresses[i], (address & 1));
            address = address >> 1;
        }
    }

    // Imposta lo stato del pin Arduino 'outputEnable'
    // LOW: high (corrente in uscita)
    // HIGH: low (no corrente in uscita)
    void setOutputEnable(const bool &state)
    {
        digitalWrite(this->outputEnable, !state);
    }

    // Imposta lo stato del pin Arduino 'writeEnable'
    // LOW: high (corrente in uscita)
    // HIGH: low (no corrente in uscita)
    void setWriteEnable(const bool &state)
    {
        digitalWrite(this->writeEnable, !state);
    }

    // Imposta lo stato dei pin Arduino per I/O dati
    // INPUT: Arduino riceve in input segnali provenienti
    // dall'esterno; dunque, il chip dovrebbe fornire segnali
    // in output.
    // OUTPUT: Arduino invia in output segnali all'esterno;
    // dunque, il chip dovrebbe accettare segnali in input.
    void setDataIO(const DataIOState &state)
    {
        switch (state)
        {
        case EEPROMManager::DataIOState::_INPUT:
            for (int i = 0; i < DATA_EXCHANGING_PIN; i++)
            {
                pinMode(this->dataIO[i], INPUT);
            }
            break;

        case EEPROMManager::DataIOState::_OUTPUT:
            for (int i = 0; i < DATA_EXCHANGING_PIN; i++)
            {
                pinMode(this->dataIO[i], OUTPUT);
            }
            break;
        }
    }

    // Metodo per effettuare il campionamento dei dati
    // in output dal chip
    double sample()
    {
        double sampleResult = 0;
        for (int i = 0; i < DATA_EXCHANGING_PIN; i++)
        {
            // Il seguente metodo dovrebbe essere invocato solo e solamente
            // quando lo stato dei pin di I/O dati è impostato su INPUT
            // (input da Arduino, output dal chip)
            sampleResult += digitalRead(this->dataIO[i]) * pow((float)2, (float)i);
        }

        return sampleResult;
    }

    // Metodo utilizzato durante la lettura dati per la costruzione di un output
    // più semplice da interpretare e "più elegante"
    String resultStringBuilder(const int &address, const double &sampleResult, const EEPROMManager::ReadMode &mode,
                               const EEPROMManager::ResultStringBuilderSpecification &specification)
    {
        String result;

        // Il seguente metodo effettua anche un'operazione di processamento
        // dell'output, utilizzando la funzione 'ceil' per effettuare un arrotondamento
        // per eccesso. Questo è reso necessario dall'imprecisione dei numeri a virgola mobile.

        // Due possibilità:
        // EEPROMManager::ResultStringBuilderSpecification::INCLUDE_HEADER_INFO:
        // insieme al risultato vero e proprio vengono incluse ulteriori informazioni,
        // che mirano ad arricchire il significato dell'output.
        // EEPROMManager::ResultStringBuilderSpecification::NOT_INCLUDE_HEADER_INFO:
        // viene solamente processato il risultato vero e proprio.
        switch (specification)
        {
        case EEPROMManager::ResultStringBuilderSpecification::INCLUDE_HEADER_INFO:
            result = "Address: 0x" + String(address, HEX) + "\n";

            switch (mode)
            {
            case ReadMode::_DEC:
                result += "DEC value: " + String((int)ceil(sampleResult));
                break;

            case ReadMode::_HEX:
                result += "HEX value: 0x" + String((int)ceil(sampleResult), HEX);
                break;

            case ReadMode::_BIN:
                result += "BIN value: b" + String((int)ceil(sampleResult), BIN);
                break;

            case ReadMode::_OCT:
                result += "OCT value: o" + String((int)ceil(sampleResult), OCT);
                break;

            case ReadMode::ALL:
                result += "DEC value: " + String((int)ceil(sampleResult)) + "\n" +
                          "HEX value: 0x" + String((int)ceil(sampleResult), HEX) + "\n" +
                          "BIN value: b" + String((int)ceil(sampleResult), BIN);
                break;
            }
            break;

        case EEPROMManager::ResultStringBuilderSpecification::NOT_INCLUDE_HEADER_INFO:
            switch (mode)
            {
            case ReadMode::_DEC:
                result = String((int)ceil(sampleResult));
                break;

            case ReadMode::_HEX:
                result = String((int)ceil(sampleResult), HEX);
                break;

            case ReadMode::_BIN:
                result = String((int)ceil(sampleResult), BIN);
                break;

            case ReadMode::_OCT:
                result = String((int)ceil(sampleResult), OCT);
                break;

            case ReadMode::ALL:
                result = String((int)ceil(sampleResult)) + "\n" +
                         String((int)ceil(sampleResult), HEX) + "\n" +
                         String((int)ceil(sampleResult), BIN);
                break;
            }
            break;
        }

        return result;
    }

    // Metodo utilizzato durante la lettura dati per la costruzione di un output
    // più semplice da interpretare e "più elegante"
    String format(const int &value, const EEPROMManager::FormatTarget &target)
    {
        String padding;

        // Il seguente metodo non fa altro che aggiungere "un numero
        // variabile di zeri" davanti ad altri valori numerici (value).
        // Questo viene utilizzato per ottenere una disposizione "a colonna"
        // "perfetta" durante la visualizzazione a schermo.
        // (solo per estetica)

        // Due possibilità:
        // EEPROMManager::FormatTarget::HEADER:
        // EEPROMManager::FormatTarget::NOT_HEADER:
        switch (target)
        {
        case EEPROMManager::FormatTarget::HEADER:
            if (value < PADDING_CHECK_ONE)
            {
                padding = "00";
            }
            else if (value < PADDING_CHECK_TWO)
            {
                padding = "0";
            }
            break;

        case EEPROMManager::FormatTarget::NOT_HEADER:
            if (value < PADDING_CHECK_ONE)
            {
                padding = "0";
            }
            break;
        }

        return padding;
    }

    // Imposta un dato/valore specificato
    void put(int data)
    {
        // Per impostare il dato/valore specificato,
        // esso viene "scomposto" nei bit che lo compongono
        // ed ogni bit, uno alla volta, viene trasmesso al chip
        // attraverso i pin Arduino di I/O dati (array dataIO).
        // Per effettuare la "scomposizione" viene utilizzato l'operatore
        // "and bitwise [&]".
        // Per avanzare "un bit alla volta" viene utilizzato l'operatore
        // "right shift [>>]"
        for (int i = 0; i < DATA_EXCHANGING_PIN; i++)
        {
            digitalWrite(this->dataIO[i], (data & 1));
            data = data >> 1;
        }
    }

    // Metodo per controllare se l'indirizzo specificato
    // è readonly (sola lettura)
    bool isReadonly(const int &address)
    {
        // Effettuo operazioni solo se i segmenti readonly (sola lettura)
        // sono attivi
        if (READONLY_SEGMENTS_ALLOWED)
        {
            // Scorro tutta la lista (array) contenente
            // gli indirizzi base dei segmenti readonly (sola lettura)
            for (int i = 0; i < READONLY_SEGMENTS; i++)
            {
                // Per ogni indirizzo base presente nella lista controllo
                // che effettivamente rappresenti un indirizzo base (di segmento)
                // valido
                if ((this->readonlySegments[i] % SEGMENT_DEPTH) == 0)
                {
                    // Se è valido, continuo per tutta l'estensione del segmento
                    for (int offset = 0; offset < SEGMENT_DEPTH; offset++)
                    {
                        // Ricerco l'indirizzo specificato
                        // Se lo trovo, significa che è readonly (sola lettura) in quanto
                        // appartiene ad un segmento readonly (sola lettura)
                        if (this->readonlySegments[i] + offset == address)
                            // Trovato
                            return true;
                    }
                }
                else
                {
                    // Se non rappresenta un indirizzo base (di segmento) valido,
                    // lo salto e continuo con il prossimo
                    continue;
                }
            }
            // Non trovato
            return false;
        }
        else
        {
            // Segmenti readonly (sola lettura) non attivi
            return false;
        }
    }
};

void setup()
{
    // Setup monitor seriale
    Serial.begin(DEFAULT_BAUD_RATE);

    // Creazione istanza di classe EEPROMManager
    EEPROMManager myEEPROM(41, 42, 22, 23, 24, 25, 26, 27, 28, 29,
                           30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40);

    // Setup iniziale dell'istanza per consentirne l'uso
    myEEPROM.init();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
