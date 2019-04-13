package main

import (
	"fmt"
	"reflect"
	"io"
	"bytes"
	"strings"
	"log"
	"ChessServer/ChessGame"
	"io/ioutil"
)

func main() {
	gen := NewCodeGenerator(`CSProtocol.h`)
	gen.ExportType(ChessGame.PiecePoint{})
	gen.ExportType(ChessGame.SyncPanelMessage{})
	gen.ExportType(ChessGame.ChatMessage{})
	gen.ExportType(ChessGame.GameOverNotice{})
	gen.ExportType(ChessGame.ServerKickYou{})
	gen.ExportTypeFromRpcMethod(&ChessGame.GameUser{})
	gen.ExportTypeFromRpcMethod(&ChessGame.GameRoom{})
	gen.ExportEnd()

	hFileContent, cppFileContent := gen.GetHeaderAndBody()
	mustWriteFile(`src/ChessClient/CSProtocol.h`, hFileContent)
	mustWriteFile(`src/ChessClient/CSProtocol.cpp`, cppFileContent)
}

func mustWriteFile(path string, content []byte) {
	oldContent, err := ioutil.ReadFile(path)
	if err == nil && bytes.Equal(oldContent, content) {
		return
	}
	err = ioutil.WriteFile(path, content, 0600)
	if err != nil {
		panic(err.Error())
	}
}

type CppCodeGenerator struct {
	wHeader         *bytes.Buffer
	wBody           *bytes.Buffer
	wNoticeHeader   *bytes.Buffer
	exportedNameMap map[string]struct{}
}

func NewCodeGenerator(dotHFileName string) *CppCodeGenerator {
	this := &CppCodeGenerator{
		wHeader:         bytes.NewBuffer(nil),
		wBody:           bytes.NewBuffer(nil),
		wNoticeHeader:   bytes.NewBuffer(nil),
		exportedNameMap: map[string]struct{}{},
	}
	this.wHeader.WriteString(`// 自动生成的代码, 不要修改!!!
#pragma once
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QJsonObject>

#include <functional>
#include <map>

struct RpcAbstract
{
public :
    virtual bool DecodeFromQByteArray(const QByteArray& bin)=0;
    virtual QByteArray EncodeToQByteArray() const=0;
	virtual QString GetMethod() const=0;
};
`)
	fmt.Fprintf(this.wBody, `// 自动生成的代码, 不要修改!!!
#include "%s"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
`, dotHFileName)
	this.wNoticeHeader.WriteString(`
class NoticeBase
{
public :
`)
	return this
}

func (this *CppCodeGenerator) ExportType(all ... interface{}) {
	for _, a := range all {
		t := reflect.TypeOf(a)
		if _, ok := this.exportedNameMap[t.Name()]; ok {
			continue
		}
		this.exportedNameMap[t.Name()] = struct{}{}
		log.Println(t.Name())
		outputType(t, this.wHeader, this.wBody, this.wNoticeHeader)
	}
}

func (this *CppCodeGenerator) ExportTypeFromRpcMethod(a interface{}) {
	aType := reflect.TypeOf(a)
	fmt.Println(`pmge6n00 `, aType.Name(), aType.String())
	for i := 0; i < aType.NumMethod(); i++ {
		method := aType.Method(i)
		methodName := method.Name
		if !strings.HasPrefix(methodName, `Rpc`) {
			continue
		}
		if method.Type.NumOut() != 1 {
			fmt.Println(`civa9ykr `, methodName, method.Type.NumIn(), method.Type.NumOut())
			continue
		}
		var inIdx int
		if method.Type.NumIn() == 2 {
			inIdx = 1
		} else if method.Type.NumIn() == 3 {
			inIdx = 2
		} else {
			fmt.Println(`hglv1ofe `, methodName, method.Type.NumIn(), method.Type.NumOut())
			continue
		}
		req := reflect.New(method.Type.In(inIdx)).Elem().Interface()
		resp := reflect.New(method.Type.Out(0)).Elem().Interface()
		this.ExportType(req, resp)
	}
}

func (this *CppCodeGenerator) ExportEnd() {
	this.wNoticeHeader.WriteString(`public:
    void OnNoticeMessage(const QString& method, const QByteArray& bin);
	void ClearNoticeCallback();
private:
    std::map<QString, std::function<void (const QByteArray& bin)> > funcMap;
};
`)
	this.wHeader.Write(this.wNoticeHeader.Bytes())
	this.wBody.WriteString(`
void NoticeBase::OnNoticeMessage(const QString& method, const QByteArray& bin)
{
    auto it = this->funcMap.find(method);
    if (it == this->funcMap.end())
    {
        return;
    }
    auto func = it->second;	// 复制出来调用才安全!!!
	func(bin);
}
void NoticeBase::ClearNoticeCallback()
{
	this->funcMap.clear();
}
`)
}

func (this *CppCodeGenerator) GetHeaderAndBody() (hFileContent, cppFileContent []byte) {
	return this.wHeader.Bytes(), this.wBody.Bytes()
}

func outputType(a reflect.Type, wHeader io.Writer, wBody io.Writer, wNoticeHeader io.Writer) {
	packetType := ChessGame.PtRpcCall
	showMethodName := ``

	wEncoder := &bytes.Buffer{}
	wDecoder := &bytes.Buffer{}
	wCons := &bytes.Buffer{}

	wnHeader := &bytes.Buffer{}
	wnBody := &bytes.Buffer{}

	if strings.HasSuffix(a.Name(), `Request`) {
		packetType = ChessGame.PtRpcCall
		showMethodName = "Rpc" + strings.TrimSuffix(a.Name(), `Request`)
	} else if strings.HasSuffix(a.Name(), `Response`) {
		packetType = ChessGame.PtRpcReply
		showMethodName = "Rpc" + strings.TrimSuffix(a.Name(), `Response`)
	} else {
		packetType = ChessGame.PtRpcNotice
		showMethodName = a.Name()

		fmt.Fprintf(wnHeader, "\tvoid Register%s(std::function<void (const %s& message)> fn);\n",
			showMethodName, showMethodName)
		fmt.Fprintf(wnBody, `
void NoticeBase::Register%s(std::function<void (const %s&)> fn)
{
    this->funcMap["%s"] = [=](const QByteArray& bin) {
        %s message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
`, showMethodName, showMethodName, showMethodName, showMethodName)
	}
	// 声明
	fmt.Fprint(wHeader, "\n")
	fmt.Fprintf(wHeader, "typedef struct %s : public RpcAbstract\n", a.Name())
	fmt.Fprintf(wHeader, "{\n")
	fmt.Fprintf(wHeader, "public :\n")
	fmt.Fprintf(wHeader, "\t%s();\n", a.Name())
	fmt.Fprintf(wHeader, "\tvirtual bool DecodeFromQByteArray(const QByteArray& bin) override final;\n")
	fmt.Fprintf(wHeader, "\tvirtual QByteArray EncodeToQByteArray() const override final;\n")
	fmt.Fprintf(wHeader, "\tvirtual QString GetMethod() const override final { return QString(\"%s\"); }\n", showMethodName)
	fmt.Fprintf(wHeader, "\tQJsonObject ToJsonObject() const;\n")
	fmt.Fprintf(wHeader, "\tbool FromJsonObject(QJsonObject obj);\n")
	fmt.Fprintf(wHeader, "public :\n")
	// Encode
	fmt.Fprintf(wEncoder, `
QByteArray %s::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "%s";
	obj["Method"] = "%s";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject %s::ToJsonObject() const
{
	QJsonObject data;
`, a.Name(), packetType, showMethodName, a.Name())

	fmt.Fprintf(wDecoder, `
bool %s::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "%s" || obj["Method"].toString() != "%s")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool %s::FromJsonObject(QJsonObject %s)
{
`, a.Name(), packetType, showMethodName, a.Name(), func() string {
		if a.NumField() == 0 {
			return ""
		}
		return "data"
	}())

	fmt.Fprintf(wCons, `%s::%s()
{
`, a.Name(), a.Name())

	for i := 0; i < a.NumField(); i++ {
		f := a.Field(i)
		switch f.Type.Kind() {
		case reflect.String:
			fmt.Fprintf(wHeader, "\tQString %s ;\n", f.Name)
			fmt.Fprintf(wEncoder, "\tdata[\"%s\"] = this->%s;\n", f.Name, f.Name)
			fmt.Fprintf(wDecoder, "\tthis->%s = data[\"%s\"].toString();\n", f.Name, f.Name)
		case reflect.Int32:
			fmt.Fprintf(wHeader, "\tint %s ;\n", f.Name)
			fmt.Fprintf(wEncoder, "\tdata[\"%s\"] = this->%s;\n", f.Name, f.Name)
			fmt.Fprintf(wDecoder, "\tthis->%s = data[\"%s\"].toInt();\n", f.Name, f.Name)
			fmt.Fprintf(wCons, "\tthis->%s = 0;\n", f.Name)
		case reflect.Bool:
			fmt.Fprintf(wHeader, "\tbool %s ;\n", f.Name)
			fmt.Fprintf(wEncoder, "\tdata[\"%s\"] = this->%s;\n", f.Name, f.Name)
			fmt.Fprintf(wDecoder, "\tthis->%s = data[\"%s\"].toBool();\n", f.Name, f.Name)
			fmt.Fprintf(wCons, "\tthis->%s = false;\n", f.Name)
		case reflect.Struct:
			fmt.Fprintf(wHeader, "\t%s %s ;\n", f.Type.Name(), f.Name)
			fmt.Fprintf(wEncoder, "\tdata[\"%s\"] = this->%s.ToJsonObject();\n", f.Name, f.Name)
			fmt.Fprintf(wDecoder, "\tthis->%s.FromJsonObject(data[\"%s\"].toObject());\n", f.Name, f.Name)
		case reflect.Slice:
			fmt.Fprintf(wHeader, "\tQVector<%s> %s ;\n", f.Type.Elem().Name(), f.Name)
			fmt.Fprintf(wEncoder, `
	{
		QJsonArray array;
		for (auto it=this->%s.begin(); it != this->%s.end(); it++)
		{
			array.append( it->ToJsonObject() );
		}
		data["%s"] = array;
	}`, f.Name, f.Name, f.Name)
			fmt.Fprintf(wDecoder, `
	{
		QJsonArray array = data["%s"].toArray();
		for (auto it=array.begin(); it != array.end(); it++)
		{
			%s temp;
			temp.FromJsonObject((*it).toObject());
			this->%s.push_back(temp);
		}
	}`, f.Name, f.Type.Elem().Name(), f.Name)
		default:
			panic(`hnri9wcl ` + f.Name)
		}
	}
	fmt.Fprintf(wCons, `}
`)
	fmt.Fprintf(wHeader, "} %s;\n", a.Name())

	fmt.Fprintf(wEncoder, `
	return data;
}
`)
	fmt.Fprintf(wDecoder, `
	return true;
}
`)
	wNoticeHeader.Write(wnHeader.Bytes())
	wBody.Write(wnBody.Bytes())
	wBody.Write(wCons.Bytes())
	wBody.Write(wEncoder.Bytes())
	wBody.Write(wDecoder.Bytes())
}
