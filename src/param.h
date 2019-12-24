#ifndef PARAM_H
#define PARAM_H

#include <functional>
#include <string>



struct SignalRange
{
	SignalRange(int _min = 0, int _max = 31)
	{
		min = _min;
		max = _max;
		
	}
	int min = 0;
	int max = 31;
	
};

class BaseParam;

class ParamLayout{
public:
	ParamLayout(){ name = "default layout";}
	ParamLayout(const char * _name): name(_name){
		
	}
	
	inline void addParam(std::shared_ptr<BaseParam> ptr){
		params.push_back(ptr);
	}
	
	
	inline void setName(const char * _name ){ name = _name;}		
	inline const char * getName(){ return name; }	
	std::vector<std::shared_ptr<BaseParam> > params; 
	
	
private:
	const char * name;
};

class BaseParam
{
	public : 
	
		BaseParam(){};		
		virtual ~BaseParam(){};
		
		inline void setName(const char * _name ){ name = _name;}		
		inline const char * getName(){ return name; }

		inline void setSignalRange(SignalRange _signal ){ signal_range = _signal;}		
		inline SignalRange getSignalRange(){ return signal_range; }
		
		inline void setUseSignalRange(bool _value)
		{
			use_signal_range = _value;
		}
		
		inline bool getUseSignalRange()
		{
			return use_signal_range;
		}
		
	private:
		const char * name = "default_param_name";
		SignalRange signal_range;
		
		bool use_signal_range = false;
};



template<typename T>
class Param : public BaseParam
{
	public:
		Param(): BaseParam(){}
		
		Param(const Param& other){}
		~Param(){};
		
		inline T getValue() { return value; }
		inline void setValue( T input ) { value = input; }
	private:
	
		T value;
		
};

class ParamSeparator : public BaseParam
{
	public:
		ParamSeparator(){}
	private:
};

class ParamMenu : public BaseParam
{
	public: 
		ParamMenu(){}
		ParamMenu(std::vector<const char *> _entries){
			int inc = 0;
			for(const char * str : _entries)
			{
				std::pair< const char *, int> entry;
				entry.first = str;
				entry.second = inc;
				
				entries.push_back(entry);
				inc++;
			}
			
			value = 0;
		}
		ParamMenu(const ParamMenu& other){}
		
		inline int getValue(){ return value; }
		inline void setValue(int _val){ value = _val; }
		
		std::vector<std::pair< const char *, int>> entries;
	private:
		int value;
		
};

class ParamButton : public BaseParam
{
	
	public:
		ParamButton() : BaseParam(){
			m_callback = [](){};
		}
		
		inline void setCallback(std::function<void()> callback)
		{
			m_callback = callback;
		}
		
		inline std::function<void()> getCallback()
		{
			return m_callback;
		}
		
	private:
	
		std::function<void()>  m_callback;
};

class ParamColor3 : public BaseParam
{
	public :
		ParamColor3() : BaseParam(){
			color = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		
		inline glm::vec3 getValue(){
			
			return color;
		}
		
		inline void setValue( glm::vec3 _color){
			color = _color;
		}
		
		glm::vec3 color ;
		
	private:
		
};


class ParamFilePath : public BaseParam
{
public:
	ParamFilePath() : BaseParam()
	{
		
	}
	
	inline void setCallback(std::function<void()> callback)
	{
		m_callback = callback;
	}
	
	inline std::function<void()> getCallback()
	{
		return m_callback;
	}
			
	inline std::string getValue()
	{
		return m_value;
	}
	
	inline void setValue(std::string value)
	{
		m_value = value;
	}
private:
	std::string m_value = "/path/";
	std::function<void()>  m_callback;
};

//~ template<>
//~ class Param <glm::vec3>{
		//~ 
		//~ public :
		//~ 
		//~ 
		//~ private :
			//~ glm::vec3 value
//~ }

#endif /* PARAM_H */ 
