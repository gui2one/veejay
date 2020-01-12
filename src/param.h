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
	
	SignalRange(const SignalRange& other)
	{
		min = other.min;
		max = other.max;
		mode = other.mode;
		multiplier = other.multiplier;
	}
	
	int min = 0;
	int max = 31;
	SignalRangeMode mode;
	float multiplier;
	
};





class BaseParam
{
	public : 
	
		BaseParam(){};		
		BaseParam(const BaseParam& other)
		{
			setName(other.getName());			
		}
		virtual ~BaseParam(){};
		
		inline void setName(const char * _name ){ name = _name;}		
		inline const char * getName() const { return name; }

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
		
		Param(const Param& other) : BaseParam(other){
			setValue(other.getValue());
		}
		~Param(){};
		
		inline T getValue() const { return value; }
		inline void setValue( T input ) { value = input; }
	private:
	
		T value;
		
};

template<>
class Param<float> : public BaseParam
{
	public:
		Param(): BaseParam(){}
		
		Param(const Param& other) : BaseParam(other){
			setValue(other.getValue());
		}
		~Param(){};
		
		inline float getValue() 	const    { return value;  }
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

class ParamInfo : public BaseParam
{
public :
	ParamInfo() : BaseParam()
	{
		
	}
	
	ParamInfo(const ParamInfo& other) : BaseParam(other)
	{
		
	}
	~ParamInfo(){};
	
	inline std::string_view getValue() const
	{
		return value;
	}
	
	inline void setValue(std::string _value)
	{
		value = _value;
	}

private :
	std::string value = "";
};

class ParamSeparator : public BaseParam
{
	public:
		ParamSeparator(){}
		ParamSeparator(const ParamSeparator& other){}
	private:
};

class ParamMenu : public BaseParam
{
	public: 
		ParamMenu() : BaseParam() {}
		ParamMenu(std::vector<const char *> _entries) : BaseParam(){
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
		ParamMenu(const ParamMenu& other) : BaseParam(other){
			
			for(auto other_entry : other.entries)
			{
				std::pair< const char *, int> entry;
				entry.first = other_entry.first;
				entry.second = other_entry.second;
				
				entries.push_back(entry);
				
			}
			
			setValue(other.getValue());			
		}
		
		inline int getValue() const { return value; }
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
		
		ParamButton(const ParamButton& other) : BaseParam(other)
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
		
	private:
	
		std::function<void()>  m_callback;
};

class ParamColor3 : public BaseParam
{
	public :
		ParamColor3() : BaseParam(){
			color = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		
		ParamColor3( const ParamColor3& other) : BaseParam(other)
		{
			setValue(other.getValue());
		}
		inline glm::vec3 getValue() const {
			
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
			
	inline std::string getValue() const
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

class ParamLayout{
public:
	ParamLayout(){ name = "default layout";}
	ParamLayout(const char * _name): name(_name){
		
	}
	
	ParamLayout(const ParamLayout& other)
	{
		setName(other.getName());
		for(auto param_ptr : other.params)
		{
			//~ Param<float> * p_float = nullptr;
			//~ Param<int> * p_int = nullptr;
			//~ Param<std::string> * p_string = nullptr;
			//~ Param<const char *> * p_text = nullptr;
			//~ Param<SignalRange> * p_signal_range = nullptr;
			//~ ParamMenu * p_menu = nullptr;
			//~ ParamSeparator * p_separator = nullptr;
			
			//~ if((p_float = dynamic_cast<Param<float> *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<Param<float> > p = std::make_shared<Param<float> >(*p_float);
				//~ addParam(p);
			//~ }
			//~ else if((p_int = dynamic_cast<Param<int> *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<Param<int> > p = std::make_shared<Param<int> >(*p_int);
				//~ addParam(p);				
			//~ }
			//~ else if((p_string = dynamic_cast<Param<std::string> *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<Param<std::string> > p = std::make_shared<Param<std::string> >(*p_string);
				//~ addParam(p);				
			//~ }			
			//~ else if((p_text = dynamic_cast<Param<const char *> *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<Param<const char *> > p = std::make_shared<Param<const char *> >(*p_text);
				//~ addParam(p);				
			//~ }
			//~ else if((p_signal_range = dynamic_cast<Param<SignalRange> *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<Param<SignalRange> > p = std::make_shared<Param<SignalRange> >(*p_signal_range);
				//~ addParam(p);				
			//~ }			
			//~ else if((p_menu = dynamic_cast<ParamMenu *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<ParamMenu > p = std::make_shared<ParamMenu >(*p_menu);
				//~ addParam(p);				
			//~ }	
			//~ else if((p_separator = dynamic_cast<ParamSeparator *>(param_ptr.get())))
			//~ {
				//~ std::shared_ptr<ParamSeparator> p = std::make_shared<ParamSeparator>(*p_separator);
				//~ addParam(p);				
			//~ }	
		}
		
	}
	
	inline void addParam(std::shared_ptr<BaseParam> ptr){
		params.push_back(ptr);
	}
	
	
	inline const char * getName() const { return name; }	
	inline void setName(const char * _name ){ name = _name;}		
	
	std::vector<std::shared_ptr<BaseParam> > params; 
	
	
private:
	const char * name;
};


#endif /* PARAM_H */ 
