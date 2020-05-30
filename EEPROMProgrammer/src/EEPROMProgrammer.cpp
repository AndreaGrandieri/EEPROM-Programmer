/**
 * @author Grandieri Andrea g.andreus02@gmail.com
 * @version 1.0
 * @file EEPROMProgrammer.cpp
 * 
 * @brief Classe per la gestione di un chip EEPROM
 * 
 * @mainpage EEPROMManager di EEPROM-Programmer
 * @section Introduzione Introduzione
 * 
 * Questa è la documentazione del codice sorgente EEPROMManager di EEPROM-Programmer.
 * Qua è dichiarata e implementata la classe EEPROMManager, la quale
 * realizza l'ambiente di gestione di un chip EEPROM.
 */

// Preprocessor directives (deprecated)
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

#define DEFAULT_BAUD_RATE 9600

/**
 * @author Grandieri Andrea g.andreus02@gmail.com
 * @class EEPROMManager
 * 
 * @brief Classe per la gestione di un chip EEPROM
 * 
 * La seguente classe fornisce tutti i metodi necessari alla realizzazione
 * di un ambiente operativo per la gestione di un chip EEPROM.
 */
class EEPROMManager
{
    /*USER FILL-IN*/
    /*
L'utente utilizzatore di tale codice dovrebbe specificare alcuni valori
relativi al chip eeprom in uso. Il seguente codice non è specializzato
per alcun chip eeprom. Non si assumono responsabilità per eventuali
danni al chip eeprom causati da erronea specificazione dei valori.
*/

    /*USER FILL-IN*/
private:
    // Timing
    // Lettura
    // Il tempo che trascorre tra l'impulso di lettura e l'effettiva presenza
    // di dati campionabili sui pin di dataIO
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il tempo
     * che trascorre tra l'invio del segnale di output enable al chip e
     * l'effettiva presenza di dati campionabili sui pin di IO
     */
    static constexpr int TIME_WAIT_READ = 1; /*microsecondi*/

    // Il tempo di recupero dopo una lettura
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante per quanto tempo
     * il chip risulta inibito dopo il completamento di una lettura (completata
     * nel momento in cui viene disattivato l'output enable)
     */
    static constexpr int TIME_RECOVERY_FROM_READ = 0; /*millisecondi*/

    // Scrittura
    // Il tempo di hold per far si di generare un segnale di inizio
    // scrittura valido per il chip
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante per quanto
     * tempo bisogna tenere attivo il segnale write enable per far effettivamente
     * iniziare un ciclo di scrittura al chip
     */
    static constexpr int TIME_HOLD_WRITE_SIGNAL = 1; /*microsecondi*/

    // Il tempo che impiega il chip fisicamente a scrivere
    // i dati nella memoria
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante quanto
     * tempo impiega un ciclo di scrittura per essere totalmente
     * completato
     */
    static constexpr int TIME_WRITE = 1; /*millisecondi*/

    // Il tempo di recupero dopo una scrittura
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante per quanto tempo
     * il chip risulta inibito dopo il completamento di una scrittura (completata
     * nel momento in cui viene completato un intero ciclo di scrittura)
     */
    static constexpr int TIME_RECOVERY_FROM_WRITE = 0; /*millisecondi*/

    // L'imprecisione, espressa in millisecondi/microsecondi, del chip
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante approssimativamente
     * la possibile imprecisione del chip in termini di temporizzazione
     */
    static constexpr int IMPRECISION = 200; /*microsecondi*/

    // Il numero di pin di addressing (scelta dell'indirizzo)
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il numero di pin
     * di indirizzamento da cui è composto il chip
     */
    static constexpr int ADDRESSING_PIN = 11;

    // Il numero di pin data (per I/O dati)
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il numero di pin
     * di IO da cui è composto il chip
     */
    static constexpr int DATA_EXCHANGING_PIN = 8;

    // Profondità dei segmenti di memoria
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante la profondità
     * di ogni singolo segmento di memoria (serie di singoli indirizzi di memoria)
     */
    static constexpr int SEGMENT_DEPTH = 16;

    // Flag indicante l'uso o non di segmenti readonly (sola lettura)
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante lo stato
     * di abilitazione del permesso di presenza di segmenti readonly
     */
    static constexpr bool READONLY_SEGMENTS_ALLOWED = false;

    // Il numero di segmenti readonly (sola lettura) usati.
    // Se non usati, valore '0'
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il numero
     * di segmenti readonly attivi
     */
    static constexpr int READONLY_SEGMENTS = 0;

    // Lista contenente un indirizzo base per ogni segmento readonly usato.
    // Se non usati, lista vuota
    /**
     * Rappresenta una direttiva del preprocessore rappresentante una lista
     * di inizializzazione di una struttura dati contingua contenente gli indirizzi
     * base dei segmenti readonly attivi
     */
#define LIST_READONLY_SEGMENTS \
    {                          \
    }

    /*PADDING*/
    // Il padding permette di visualizzare un output "più elegante"
    // Add more padding checkers if needed following this rule:
    // PADDING_CHECK_n = 16^n
private:
    /**
     * Rappresenta una direttiva del preprocessore moderna utilizzata per
     * eseguire il padding dell'output
     * 
     * @see EEPROMManager::format()
     */
    static constexpr int PADDING_CHECK_ONE = 16; /*16^1*/

    /**
     * Rappresenta una direttiva del preprocessore moderna utilizzata per
     * eseguire il padding dell'output
     *
     * @see EEPROMManager::format()
     */
    static constexpr int PADDING_CHECK_TWO = 256; /*16^2*/

    /*DO NOT MODIFY*/
private:
    // Il numero di indirizzi totali (non tutti raggiungibili)
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il numero
     * di indirizzi totali (i quali non sono tutti raggiungibili. Quelli raggiungibili
     * risiedono nel range 0 -- (ADDRESSES - 1))
     */
    static constexpr int ADDRESSES = pow(2, ADDRESSING_PIN); /*DEDUCED*/

    // L'indirizzo "più basso" (con valore algebricamente più alto) raggiungibile
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante l'indirizzo
     * più basso (con valore algebricamente più alto) raggiungibile
     */
    static constexpr int LOWEST_ADDRESS = ADDRESSES - 1; /*DEDUCED*/

    // L'indirizzo "più alto" (con valore algebricamente più basso) raggiungibile
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante l'indirizzo
     * più alto (con valore algebricamente più basso) raggiungibile
     */
    static constexpr int HIGHEST_ADDRESS = 0;

    // Il numero di valori validi realizzabili con il numero
    // di bit del chip (non tutti utilizzabili)
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il numero di valori
     * validi realizzabili con il numero di bit del chip (non tutti utilizzabili. Quelli realizzabili
     * risiedono nel range 0 -- (VALID_DATA_VALUES - 1))
     */
    static constexpr int VALID_DATA_VALUES = pow(2, DATA_EXCHANGING_PIN); /*DEDUCED*/

    // Il valore algebricamente più basso utilizzabile
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il valore algebricamente
     * più basso realizzabile con il numero di bit del chip
     */
    static constexpr int LOWEST_VALID_DATA_VALUE = 0;

    // Il valore algebricamente più alto utilizzabile
    /**
     * Rappresenta una direttiva del preprocessore moderna indicante il valore algebricamente
     * più alto realizzabile con il numero di bit del chip
     */
    static constexpr int HIGHEST_VALID_DATA_VALUE = VALID_DATA_VALUES - 1; /*DEDUCED*/

    /*DO NOT MODIFY*/
private:
    // Pin arduino per l'output enable
    /**
     * Rappresenta il pin utilizzato per l'output del segnale
     * di output enable verso il chip
     */
    const int outputEnable;

    // Pin arduino per il write enable
    /**
     * Rappresenta il pin utilizzato per l'output del segnale
     * di write enable verso il chip
     */
    const int writeEnable;

    // [Array] Pin arduino per la scelta dell'indirizzo
    /**
     * Rappresenta un array di pin utilizzati per l'addressing
     * verso il chip
     */
    const int *addresses;

    // [Array] Pin arduino per I/O dati
    /**
     * Rappresenta un array di pin utilizzati per l'IO di dati
     * da e verso il chip
     */
    const int *dataIO;

    // [Array] Segmenti readonly (sola lettura)
    /**
     * Rappresenta un array di indirizzi base dei segmenti 
     * readonly attivi
     */
    const int *readonlySegments;

    // Flag indicante lo stato di inizializzazione di "this"
    /**
     * Rappresenta un flag booelano indicante lo stato di inizializzazione
     * dell'oggetto "this" in questione
     */
    bool hasBeenInit;

public:
    // Enumerazione per indicare la modalità di interpretazione
    // dei dati letti
    /**
     * @enum ReadMode
     * 
     * @brief Enumerazione rappresentante le diverse modalità di interpretazione
     * dei dati letti
     * 
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::resultStringBuilder()
     */
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
    /**
     * @enum DataIOState
     * 
     * @brief Enumerazione rappresentante le diverse modalità di funzionamento
     * attribuibili ai pin IO
     * 
     * @see EEPROMManager::dataIO
     */
    enum class DataIOState
    {
        _INPUT,
        _OUTPUT
    };

    // Enumerazione per indicare la modalità di clear del chip
    /**
     * @enum ClearMode
     * 
     * @brief Enumerazione rappresentante le diverse modalità di formattazione
     * della memoria del chip
     * 
     * @see EEPROMManager::clear()
     * @see EEPROMManager::hardClear()
     */
    enum class ClearMode
    {
        LOWEST_VALUE_FILL,
        HIGHEST_VALUE_FILL
    };

private:
    /**
     * @enum ResultStringBuilderSpecification
     * 
     * @brief Enumerazione rappresentante le diverse modalità di formattazione
     * dell'output
     * 
     * @see EEPROMMangaer::resultStringBuilder()
     */
    enum class ResultStringBuilderSpecification
    {
        INCLUDE_HEADER_INFO,
        NOT_INCLUDE_HEADER_INFO
    };

    // Enumerazione per indicare la modalità di applicazione
    // del padding
    /**
     * @enum FormatTarget
     * 
     * @brief Enumerazione rappresentante i diversi target interessati
     * dalla formattazione dell'output
     * 
     * @see EEPROMManager::format()
     */
    enum class FormatTarget
    {
        HEADER,
        NOT_HEADER
    };

public:
    // Costruttore di default
    EEPROMManager() = delete;

    // Costruttore parametrico
    /**
     * @brief Costruttore 
     * 
     * Inizializza tutte le risorse necessarie ad un oggetto di questa classe
     * 
     * @param outputEnable il pin utilizzato per l'output del segnale di output enable verso il chip
     * @param writeEnable il pin utilizzato per l'output del segnale di write enable verso il chip
     * @param a0 il pin di addressing a0 verso il chip
     * @param a1 il pin di addressing a1 verso il chip
     * @param a2 il pin di addressing a2 verso il chip
     * @param a3 il pin di addressing a3 verso il chip
     * @param a4 il pin di addressing a4 verso il chip
     * @param a5 il pin di addressing a5 verso il chip
     * @param a6 il pin di addressing a6 verso il chip
     * @param a7 il pin di addressing a7 verso il chip
     * @param a8 il pin di addressing a8 verso il chip
     * @param a9 il pin di addressing a9 verso il chip
     * @param a10 il pin di addressing a10 verso il chip
     * @param io0 il pin di IO di dati io0 da e verso il chip
     * @param io1 il pin di IO di dati io1 da e verso il chip
     * @param io2 il pin di IO di dati io2 da e verso il chip
     * @param io3 il pin di IO di dati io3 da e verso il chip
     * @param io4 il pin di IO di dati io4 da e verso il chip
     * @param io5 il pin di IO di dati io5 da e verso il chip
     * @param io6 il pin di IO di dati io6 da e verso il chip
     * @param io7 il pin di IO di dati io7 da e verso il chip
     * 
     * @see EEPROMManager::outputEnable
     * @see EEPROMManager::writeEnable
     * @see EEPROMManager::addresses
     * @see EEPROMManager::dataIO
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::READONLY_SEGMENTS_ALLOWED
     * @see EEPROMManager::readonlySegments
     * @see EEPROMManager::LIST_READONLY_SEGMENTS
     */
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
    /**
     * @brief Distruttore
     * 
     * Finalizza tutte le risorse usate da un oggetto di questa classe
     * 
     * @see EEPROMManager::addresses
     * @see EEPROMManager::dataIO
     * @see EEPROMManager::readonlySegments
     */
    ~EEPROMManager()
    {
        delete[] this->addresses;
        delete[] this->dataIO;
        delete[] this->readonlySegments;
    }

    // Metodo di inizializzazione di "this"
    /**
     * @brief Inizializzatore stabile
     * 
     * Assegna valori stabili alle risorse HW/SW che necessitano di un valore forzato
     * per il corretto funzionamento-
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::setOutputEnable()
     * @see EEPROMManager::setWriteEnable()
     * @see EEPROMManager::outputEnable
     * @see EEPROMManager::writeEnable
     * @see EEPROMManager::ADDRESSING_PIN
     * @see EEPROMManager::addresses
     */
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
    /**
     * @brief Effettua la lettura di un singolo indirizzo di memoria
     * 
     * Il seguente metodo effettua la lettura di un singolo indirizzo di memoria
     * e stampa il risultato utilizzando l'output standard.
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @param address L'indirizzo di memoria da leggere
     * @param mode La modalità di interpretazione dei dati letti
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::HIGHEST_ADDRESS
     * @see EEPROMManager::LOWEST_ADDRESS
     * @see EEPROMManager::setDataIO()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::setAddress()
     * @see EEPROMManager::setOutputEnable()
     * @see EEPROMManager::TIME_WAIT_READ
     * @see EEPROMManager::IMPRECISION
     * @see EEPROMManager::sample()
     * @see EEPROMManager::resultStringBuilder()
     * @see EEPROMManager::ResultStringBuilderSpecification
     * @see EEPROMManager::TIME_RECOVERY_FROM_READ
     */
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
                result = this->resultStringBuilder(address, sampleResult, mode, EEPROMManager::ResultStringBuilderSpecification::INCLUDE_HEADER_INFO);

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
    /**
     * @brief Effettua la lettura di un intero segmento di memoria
     * 
     * Il seguente metodo effettua la lettura di un intero segmento di memoria
     * e stampa il risultato utilizzando l'output standard.
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @param baseAddress L'indirizzo base del segmento di memoria da leggere
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::HIGHEST_ADDRESS
     * @see EEPROMManager::LOWEST_ADDRESS
     * @see EEPROMManager::setDataIO()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::setAddress()
     * @see EEPROMManager::setOutputEnable()
     * @see EEPROMManager::TIME_WAIT_READ
     * @see EEPROMManager::IMPRECISION
     * @see EEPROMManager::sample()
     * @see EEPROMManager::resultStringBuilder()
     * @see EEPROMManager::ResultStringBuilderSpecification
     * @see EEPROMManager::TIME_RECOVERY_FROM_READ
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::format()
     * @see EEPROMManager::FormatTarget
     */
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
    /**
     * @brief Effettua la lettura dell'intera memoria del chip
     * 
     * Il seguente metodo effettua la lettura dell'intera memoria del chip
     * e stampa il risultato utilizzando l'output standard.
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::ADDRESSES
     * @see EEPROMManager::readSegment()
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::readSegment()
     */
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
    /**
     * @brief Effettua la scrittura di un singolo indirizzo di memoria
     * 
     * Il seguente metodo effettua la scrittura di un singolo indirizzo di memoria.
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @param address L'indirizzo di memoria da scrivere
     * @param data Il valore da scrivere
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::HIGHEST_ADDRESS
     * @see EEPROMManager::LOWEST_ADDRESS
     * @see EEPROMManager::setDataIO()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::setAddress()
     * @see EEPROMManager::setWriteEnable()
     * @see EEPROMManager::IMPRECISION
     * @see EEPROMManager::put()
     * @see EEPROMManager::LOWEST_VALID_DATA_VALUE
     * @see EEPROMManager::HIGHEST_VALID_DATA_VALUE
     * @see EEPROMManager::isReadonly()
     * @see EEPROMManager::TIME_HOLD_WRITE_SIGNAL
     * @see EEPROMManager::TIME_RECOVERY_FROM_WRITE
     * @see EEPROMManager::TIME_WRITE
     */
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
    /**
     * @brief Effettua la scrittura di un intero segmento di memoria
     * 
     * Il seguente metodo effettua la scrittura di un intero segmento di memoria.
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @param baseAddress L'indirizzo base del segmento di memoria da scrivere
     * @param data Il valore da scrivere
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::HIGHEST_ADDRESS
     * @see EEPROMManager::LOWEST_ADDRESS
     * @see EEPROMManager::setDataIO()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::setAddress()
     * @see EEPROMManager::setWriteEnable()
     * @see EEPROMManager::IMPRECISION
     * @see EEPROMManager::put()
     * @see EEPROMManager::LOWEST_VALID_DATA_VALUE
     * @see EEPROMManager::HIGHEST_VALID_DATA_VALUE
     * @see EEPROMManager::isReadonly()
     * @see EEPROMManager::TIME_HOLD_WRITE_SIGNAL
     * @see EEPROMManager::TIME_RECOVERY_FROM_WRITE
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::TIME_WRITE
     */
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
    /**
     * @brief Effettua la scrittura dell'intera memoria del chip
     * 
     * Il seguente metodo effettua la scrittura dell'intera memoria del chip.
     * 
     * @param data Il valore da scrivere
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::LOWEST_VALID_DATA_VALUE
     * @see EEPROMManager::HIGHEST_VALID_DATA_VALUE
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::ADDRESSES
     * @see EEPROMManager::writeSegment()
     */
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
    /**
     * @brief Effettua la formattazione della memoria del chip
     * 
     * Il seguente metodo effettua la formattazione della memoria del chip,
     * causando la perdita di tutte le informazioni in esso contenute
     * 
     * @param mode La modalità di formattazione della memoria del chip
     * 
     * @attention Il processo di formattazione "clear" causa la perdita
     * di tutte le informazioni contenute nella memoria del chip, eccetto per
     * i segmenti readonly
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::writeAll()
     * @see EEPROMManager::ClearMode
     */
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
    /**
     * @brief Effettua la formattazione della memoria del chip
     * 
     * Il seguente metodo effettua la formattazione della memoria del chip,
     * causando la perdita di tutte le informazioni in esso contenute
     * 
     * @param mode La modalità di formattazione della memoria del chip
     * 
     * @attention Il processo di formattazione "hardClear" causa la perdita
     * di tutte le informazioni contenute nella memoria del chip, compresi i
     * i segmenti readonly
     * 
     * @pre L'inizializzazione deve essere già stata eseguita
     * 
     * @see EEPROMManager::hasBeenInit
     * @see EEPROMManager::HIGHEST_ADDRESS
     * @see EEPROMManager::LOWEST_ADDRESS
     * @see EEPROMManager::setDataIO()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::setAddress()
     * @see EEPROMManager::setWriteEnable()
     * @see EEPROMManager::IMPRECISION
     * @see EEPROMManager::put()
     * @see EEPROMManager::LOWEST_VALID_DATA_VALUE
     * @see EEPROMManager::HIGHEST_VALID_DATA_VALUE
     * @see EEPROMManager::isReadonly()
     * @see EEPROMManager::TIME_HOLD_WRITE_SIGNAL
     * @see EEPROMManager::TIME_RECOVERY_FROM_WRITE
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::TIME_WRITE
     */
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
            }
        }
    }

private:
    // Seleziona l'indirizzo specificato
    /**
     * @brief Seleziona l'indirizzo specificato
     * 
     * Il seguente metodo seleziona l'indirizzo specificato attraverso
     * un processo di output di segnali sui bus in output dal controllore e
     * in input al chip sui pin di addressing.
     * 
     * @param address L'indirizzo di memoria da selezionare
     * 
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::readSegment()
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     * @see EEPROMManager::hardClear()
     * @see EEPROMManager::ADDRESSING_PIN
     * @see EEPROMManager::addresses
     */
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
    /**
     * @brief Cambia lo stato del segnale di output enable verso il chip
     * 
     * Il seguente metodo cambia lo stato del segnale di output enable
     * verso il chip
     * 
     * @param state Lo stato da impostare per il segnale di output enable verso il chip
     * 
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::readSegment()
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     * @see EEPROMManager::hardClear()
     * @see EEPROMManager::init()
     * @see EEPROMManager::outputEnable
     */
    void setOutputEnable(const bool &state)
    {
        digitalWrite(this->outputEnable, !state);
    }

    // Imposta lo stato del pin Arduino 'writeEnable'
    // LOW: high (corrente in uscita)
    // HIGH: low (no corrente in uscita)
    /**
     * @brief Cambia lo stato del segnale di write enable verso il chip
     * 
     * Il seguente metodo cambia lo stato del segnale di write enable
     * verso il chip
     * 
     * @param state Lo stato da impostare per il segnale di write enable verso il chip
     * 
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     * @see EEPROMManager::hardClear()
     * @see EEPROMManager::init()
     * @see EEPROMManager::writeEnable
     */
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
    /**
     * @brief Cambia lo stato dei pin IO dal punto di vista del controllore
     * 
     * Il seguente metodo cambia lo stato dei pin IO dal punto
     * di vista del controllore:
     * - EEPROMManager::DataIOState::_INPUT: indica che il controllore riceve
     * segnali provenienti in input dall'esterno, dunque, il chip
     * dovrebbe fornire segnali in output
     * - EEPROMManager::DataIOState::_OUTPUT: indica che il controllore invia
     * segnali in output verso l'esterno, dunque, il chip
     * dovrebbe accettare segnali in input.
     * 
     * @param state Lo stato da impostare per i pin IO
     * 
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::readSegment()
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     * @see EEPROMManager::hardClear()
     * @see EEPROMManager::DataIOState
     * @see EEPROMManager::DATA_EXCHANGING_PIN
     * @see EEPROMManager::dataIO
     */
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
    /**
     * @brief Effettua il campionamento dei dati in output dal chip
     * 
     * Il seguente metodo effettua il campionamento dei dati in output dal
     * chip. Dunque, il chip dovrebbe fornire segnali in output e il controllore
     * accettare segnali in input.
     * 
     * @pre I pin IO del controllore devono essere impostati su modalità
     * EEPROMManager::DataIOState::_INPUT
     * 
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::readSegment()
     * @see EEPROMManager::DATA_EXCHANGING_PIN
     * @see EEPROMManager::dataIO
     */
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
    /**
     * @brief Formatta i dati da visualizzare per ottenere un output
     * a schermo più elegante e semplice da interpretare e permette di effettuare
     * la conversione dei dati da visualizzare in basi differenti
     * 
     * Il seguente metodo formatta i dati da visualizzare per ottenere un output
     * a schermo più elegante e semplice da interpretare. Inoltre
     * permette di effettuare la conversione dei dati da visualizzare nelle seguenti basi:
     * - DEC
     * - HEX
     * - OCT
     * - BIN
     * 
     * @param address L'indirizzo di memoria da includere come informazione nell'output formattato
     * @param sampleResult Il valore letto dal chip da includere come informazione nell'output formattato
     * @param mode La modalità di interpretazione dei dati letti: la base
     * @param specification Indica la presenza o meno di informazioni aggiuntive poste ad arricchire il
     * significato dell'output
     * 
     * @see EEPROMManager::ResultStringBuilderSpecification
     * @see EEPROMManager::ReadMode
     * @see EEPROMManager::readAddress()
     * @see EEPROMManager::readSegment()
     */
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
    /**
     * @brief Formatta i dati da visualizzare per ottenere un output
     * a schermo più elegante e semplice da interpretare
     * 
     * Il seguente metodo formatta i dati da visualizzare per ottenere un output
     * a schermo più elegante e semplice da interpretare.
     * Viene utilizzata la tecnica del padding, la quale consiste nell'aggiunta di "un
     * numero variabile di zeri" davanti ad altri valori numerici per ottenere
     * un incolonnamento "perfetto".
     * 
     * @param value Il valore numerico al quale applicare un eventuale padding
     * @param target Il target del padding
     * 
     * @see EEPROMManager::FormatTarget
     * @see EEPROMManager::PADDING_CHECK_ONE
     * @see EEPROMManager::PADDING_CHECK_TWO
     */
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
    /**
     * @brief Imposta il valore specificato sui pin IO
     * 
     * Il seguente metodo imposta il valore specificato
     * sui pin IO, scomponendolo nei singoli bit da cui è 
     * composto e assegnando ciascuno di essi ad un rispettivo
     * pin IO (rispettando la sequenza MSB - LSB). Dunque, ogni singolo
     * bit avrà il proprio bus e il proprio pin IO.
     * 
     * @param state Il valore da impostare
     * 
     * @see EEPROMManager::DATA_EXCHANGING_PIN
     * @see EEPROMManager::dataIO
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     * @see EEPROMManager::hardClear()
     */
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
    /**
     * @brief Controlla se l'indirizzo base specificato
     * si riferisce ad un segmento readonly
     * 
     * Il seguente metodo controlla se l'indirizzo base specificato
     * si riferisce ad un segmento readonly.
     * 
     * @param baseAddress L'indirizzo base da controllare
     * 
     * @see EEPROMManager::READONLY_SEGMENTS_ALLOWED
     * @see EEPROMManager::READONLY_SEGMENTS
     * @see EEPROMManager::readonlySegments
     * @see EEPROMManager::SEGMENT_DEPTH
     * @see EEPROMManager::writeAddress()
     * @see EEPROMManager::writeSegment()
     */
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
