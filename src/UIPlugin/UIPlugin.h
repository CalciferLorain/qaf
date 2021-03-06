#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "uiplugin_global.h"
#include "AbstractPlugin.h"

using namespace QAF;

class UIPLUGIN_EXPORT UIPluginFactory :public AbstractPluginFactory
{
public:
	virtual ObjectPtr<AbstractPlugin> create(QAFCore* core);
	virtual PluginType getType();
	virtual QString getVersion();
	virtual QString getDesc();
	virtual QString getAuthor();
	virtual bool isAutoLoad();
};

class  UIPlugin :public AbstractPlugin
{
public:
	UIPlugin();
	~UIPlugin();

	virtual void install();
	virtual void uninstall();

private:

};

#endif // TESTPLUGIN_H