#ifndef SINGLETON_H
#define SINGLETON_H

abstract class Singleton : public QObject
{
	Q_OBJECT

  private:
	explicit Singleton(QObject *parent = nullptr);
	~Singleton();

  public:
	static Singleton &instance()
	{
		static Singleton *_instance = nullptr;
		if (_instance == nullptr)
		{
			_instance = new Singleton();
		}
		return *_instance;
	}
};

#endif // SINGLETON_H
