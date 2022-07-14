#ifndef MERKAARTOR_STYLE_TAGSELECTOR_H_
#define MERKAARTOR_STYLE_TAGSELECTOR_H_

class IFeature;

#include <QtCore/QString>
#include <QRegularExpression>
#include <QList>
#include <QStringList>

#include <QDateTime>

enum TagSelectorMatchResult {
    TagSelect_NoMatch,
    TagSelect_Match,
    TagSelect_DefaultMatch
};

enum TagSelectorSpecialKey {
    TagSelectKey_None,
    TagSelectKey_Id,
    TagSelectKey_User,
    TagSelectKey_Time,
    TagSelectKey_Version,
    TagSelectKey_ZoomLevel,
    TagSelectKey_PixelPerM,
    TagSelectKey_Dirty,
    TagSelectKey_Uploaded
};

enum TagSelectorSpecialValue {
    TagSelectValue_None,
    TagSelectValue_Empty
};

class TagSelector
{
    public:
        virtual ~TagSelector() = 0;

        virtual TagSelector* copy() const = 0;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const = 0;
        virtual QString asExpression(bool Precedence) const = 0;

        static TagSelector* parse(const QString& Expression);
        static TagSelector* parse(const QString& Expression, int& idx);
};

class TagSelectorOperator : public TagSelector
{
    enum Ops {
        EQ,
        NE,
        GT,
        LT,
        LE,
        GE
    };
    public:
        TagSelectorOperator(const QString& key, const QString& oper, const QString& value);

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        TagSelectorMatchResult evaluateVal(const QString& val) const;

        QRegularExpression rx;
        QString Key, Oper, Value;
        Ops theOp;
        qreal numValue;
        QDateTime dtValue;
        bool UseFullRegExp;
        TagSelectorSpecialKey specialKey;
        TagSelectorSpecialValue specialValue;
        bool boolVal, valB;
        bool okval;
        qreal valN;
};

class TagSelectorIsOneOf : public TagSelector
{
    public:
        TagSelectorIsOneOf(const QString& key, const QStringList& values);

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        QList<QRegularExpression> rxv;
        QStringList exactMatchv;
        QString Key;
        QStringList Values;
        TagSelectorSpecialKey specialKey;
        TagSelectorSpecialValue specialValue;
};

class TagSelectorTypeIs : public TagSelector
{
    public:
        TagSelectorTypeIs(const QString& type);

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        QString Type;
};

class TagSelectorHasTags : public TagSelector
{
    public:
        TagSelectorHasTags();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        QStringList TechnicalTags;
};

class TagSelectorOr : public TagSelector
{
    public:
        TagSelectorOr(const QList<TagSelector*> Terms);
        virtual ~TagSelectorOr();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        QList<TagSelector*> Terms;
};

class TagSelectorAnd : public TagSelector
{
    public:
        TagSelectorAnd(const QList<TagSelector*> Terms);
        virtual ~TagSelectorAnd();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        QList<TagSelector*> Terms;
};

class TagSelectorNot : public TagSelector
{
    public:
        TagSelectorNot(TagSelector* Term);
        virtual ~TagSelectorNot();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        TagSelector* Term;
};

class TagSelectorParent : public TagSelector
{
    public:
        TagSelectorParent(TagSelector* Term);
        virtual ~TagSelectorParent();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        TagSelector* Term;
};

class TagSelectorFalse : public TagSelector
{
    public:
        TagSelectorFalse();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;
};

class TagSelectorTrue : public TagSelector
{
    public:
        TagSelectorTrue();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;
};

class TagSelectorDefault : public TagSelector
{
    public:
        TagSelectorDefault(TagSelector* Term);
        virtual ~TagSelectorDefault();

        virtual TagSelector* copy() const;
        virtual TagSelectorMatchResult matches(const IFeature* F, qreal PixelPerM) const;
        virtual QString asExpression(bool Precedence) const;

    private:
        TagSelector* Term;
};


#endif
