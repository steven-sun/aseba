#ifndef THYMIO_INTERMEDIATE_REPRESENTATION_H
#define THYMIO_INTERMEDIATE_REPRESENTATION_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

namespace Aseba
{
	using namespace std;
	
	enum ThymioIRButtonName 
	{
		THYMIO_BUTTONS_IR = 0,
		THYMIO_PROX_IR,
		THYMIO_PROX_GROUND_IR,
		THYMIO_TAP_IR,
		THYMIO_CLAP_IR,
		THYMIO_MOVE_IR,
		THYMIO_COLOR_TOP_IR,
		THYMIO_COLOR_BOTTOM_IR,
		THYMIO_SOUND_IR,
		THYMIO_MEMORY_IR
	};
	
	enum ThymioIRErrorCode
	{
		THYMIO_NO_ERROR = 0,
		THYMIO_MISSING_EVENT,
		THYMIO_MISSING_ACTION,
		THYMIO_EVENT_MULTISET,
		THYMIO_INVALID_CODE
	};
	
	enum ThymioIRErrorType
	{
		THYMIO_NO_TYPE_ERROR = 0,
		THYMIO_SYNTAX_ERROR,
		THYMIO_TYPE_ERROR,
		THYMIO_CODE_ERROR
	};
	
	class ThymioIRVisitor;
	
	/**
		An intermediate representation for an "event" or "action" card.
		This is the parallel to ThymioButton but without any graphic
		dependencies. The two classes have similar members such as
		setValue()/getValue()/getStateFilter()/...
	*/
	class ThymioIRButton 
	{
	public:
		ThymioIRButton(int size=0, ThymioIRButtonName n=THYMIO_BUTTONS_IR);
		~ThymioIRButton();

		int valuesCount() const;
		int getValue(int i) const;
		void setValue(int i, int status);
		
		int getStateFilter() const;
		int getStateFilter(int i) const;
		void setStateFilter(int s);

		ThymioIRButtonName getName() const;
		void setBasename(wstring n);
		wstring getBasename() const;
		
		bool isEventButton() const;
		bool isSet() const;

		void accept(ThymioIRVisitor *visitor);

	private:
		vector<int> values;
		int memory;
		ThymioIRButtonName name;
		wstring basename;
	};
	
	class ThymioIRButtonSet
	{
	public:
		ThymioIRButtonSet(ThymioIRButton *event=0, ThymioIRButton *action=0);
		
		void setEventButton(ThymioIRButton *event);
		void setActionButton(ThymioIRButton *action);
		
		ThymioIRButton *getEventButton();
		ThymioIRButton *getActionButton();
		
		bool hasEventButton() const;
		bool hasActionButton() const;
	
		void accept(ThymioIRVisitor *visitor);
	
	private:
		ThymioIRButton *eventButton;
		ThymioIRButton *actionButton;
	};
	
	class ThymioIRVisitor
	{
	public:
		ThymioIRVisitor() : errorCode(THYMIO_NO_ERROR) {}
		
		virtual void visit(ThymioIRButton *button);
		virtual void visit(ThymioIRButtonSet *buttonSet);

		ThymioIRErrorCode getErrorCode() const;
		bool isSuccessful() const;

	protected:
		ThymioIRErrorCode errorCode;
		wstring toWstring(int val);
	};

	class ThymioIRTypeChecker : public ThymioIRVisitor
	{
	public:
		ThymioIRTypeChecker() : ThymioIRVisitor(), activeActionName(THYMIO_BUTTONS_IR) {}
		~ThymioIRTypeChecker();
		
		virtual void visit(ThymioIRButton *button);
		virtual void visit(ThymioIRButtonSet *buttonSet);		
		
		void reset();
		void clear();
		
	private:
		ThymioIRButtonName activeActionName;
		
		multimap<wstring, ThymioIRButton*> moveHash;
		multimap<wstring, ThymioIRButton*> colorTopHash;
		multimap<wstring, ThymioIRButton*> colorBottomHash;
		multimap<wstring, ThymioIRButton*> soundHash;
		multimap<wstring, ThymioIRButton*> memoryHash;

		set<ThymioIRButtonName> tapSeenActions;
		set<ThymioIRButtonName> clapSeenActions;
	};

	class ThymioIRSyntaxChecker : public ThymioIRVisitor
	{
	public:
		ThymioIRSyntaxChecker() : ThymioIRVisitor() {}
		~ThymioIRSyntaxChecker() {}
		
		virtual void visit(ThymioIRButton *button);
		virtual void visit(ThymioIRButtonSet *buttonSet); 
	
		void clear() {}
	};

	class ThymioIRCodeGenerator : public ThymioIRVisitor
	{
	public:
		ThymioIRCodeGenerator();
		~ThymioIRCodeGenerator();
		
		virtual void visit(ThymioIRButton *button);
		virtual void visit(ThymioIRButtonSet *buttonSet);

		vector<wstring>::const_iterator beginCode() const { return generatedCode.begin(); }
		vector<wstring>::const_iterator endCode() const { return generatedCode.end(); }
		void reset();
		void clear();
		void addInitialisation();
		
		int buttonToCode(int id) const;
		
	private:
		map<ThymioIRButtonName, pair<int, int> > editor;

		vector<wstring> generatedCode;
		bool advancedMode;
		bool useSound;
		bool useMicrophone;
		vector<wstring> directions;
		int currentBlock;
		bool inIfBlock;
		vector<int> buttonToCodeMap;
	};

	class ThymioCompiler 
	{
	public:
		ThymioCompiler();
		~ThymioCompiler();
		
		void compile();
		void generateCode();

		void addButtonSet(ThymioIRButtonSet *set);
		void insertButtonSet(int row, ThymioIRButtonSet *set);
		void removeButtonSet(int row);
		void replaceButtonSet(int row, ThymioIRButtonSet *set);
		void swap(int row1, int row2);
		int buttonToCode(int id) const;

		ThymioIRErrorCode getErrorCode() const;
		bool isSuccessful() const;
		int getErrorLine() const;

		vector<wstring>::const_iterator beginCode() const;
		vector<wstring>::const_iterator endCode() const; 

		void clear();

	private:
		vector<ThymioIRButtonSet*> buttonSet;
		ThymioIRTypeChecker   typeChecker;
		ThymioIRSyntaxChecker syntaxChecker;
		ThymioIRCodeGenerator codeGenerator;
		
		ThymioIRErrorType errorType;

		int errorLine;
	};

}; // Aseba

#endif