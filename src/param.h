#ifndef PARAM_H
#define PARAM_H

#include <functional>
#include <string>

class BaseParam;
struct SignalRange;

enum SignalRangeMode
{
	SignalRangeMode_MULTIPLY,
	SignalRangeMode_ADD,
	SignalRangeMode_AROUND
};

struct SignalRange
{
	SignalRange(int _min = 0, int _max = 31, SignalRangeMode _mode = SignalRangeMode_ADD, float _multiplier = 1.0f)
	{
		min = _min;
		max = _max;
		mode = _mode;
		multiplier = _multiplier;		
	}
	
	int min = 0;
	int max = 31;
	SignalRangeMode mode;
	float multiplier;
	
};



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

template<>
class Param<float> : public BaseParam
{
	public:
		Param(): BaseParam(){}
		
		Param(const Param& other){}
		~Param(){};
		
		inline float getValue() 			 { return value;  }
		inline void  setValue( float input ) { value = input; }
		
		inline float getFilteredValue(float * fft_maximums)
		{
			if( getUseSignalRange() )
			{
				SignalRange signal_range = getSignalRange();
				size_t min = signal_range.min;
				size_t max = signal_range.max;
				
				float accum = 0.0f;
			
				for(size_t i = min; i <= max; i++)
				{
					accum += fft_maximums[i];
				}
				
				accum /= (float)(max - min);
				accum *= 10.0f;	
				
				accum *= signal_range.multiplier;
				
				switch(signal_range.mode) 
				{
					case SignalRangeMode_MULTIPLY : 
						return getValue() * accum;
						break;

					case SignalRangeMode_ADD : 
						return getValue() + accum;
						break;

					case SignalRangeMode_AROUND : 
						return getValue() + (accum * signal_range.multiplier * 2.0) - signal_range.multiplier;
						break;
					default : 
						break;
				}
				return getValue() * accum;
			}else{
				return getValue();
			}	
		}
	private:
	
		float value;
		
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
