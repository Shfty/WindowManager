#ifndef SINGLETON_H
#define SINGLETON_H

class Singleton
{
	private:
		Singleton() {}

	public:
		static Singleton & instance() {
			static Singleton * _instance = nullptr;
			if ( _instance == nullptr ) {
				_instance = new Singleton();
			}
			return *_instance;
		}
};

#endif // SINGLETON_H
