//	Copyright (C) 2014, 2018 Vaptistis Anogeianakis <nomad@cornercase.gr>
/*
 *	This file is part of LostArt.
 *
 *	LostArt is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	LostArt is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with LostArt.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SYMBOLIC_COMPUTATION_H
#define SYMBOLIC_COMPUTATION_H

#include <limits>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <utility>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <functional>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "symbol table.hpp"

namespace Symbolic
{
	// perhaps wrap following namespaces in namespace "Expressions"?

	namespace FreeForms
	{
		// namespace declarations to ease development with Spirit
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		namespace lex = boost::spirit::lex;
		namespace phoenix = boost::phoenix;

		namespace OpTags
		{
			using priority_type = int;
			enum class Associativity {LEFT,RIGHT};
			enum class Child{LEFT,RIGHT};

			struct OpTraits
			{
				// Fields
				char symbol;
				priority_type priority;
				Associativity associativity;

				// Constructors
				OpTraits(char symbol, priority_type priority, Associativity associativity)
					:symbol(symbol), priority(priority), associativity(associativity)
				{
					// empty body
				} // end OpTraits constructor

				// Methods
				OpTraits &setSymbol(char symbol)
				{
					this->symbol = symbol;
					return *this;
				} // end method setSymbol

				OpTraits &setPriority(priority_type priority)
				{
					this->priority = priority;
					return *this;
				} // end method setPriority

				OpTraits &setAssociativity(Associativity associativity)
				{
					this->associativity = associativity;
					return *this;
				} // end method setAssociativity
			}; // end struct OpTraits

			constexpr priority_type noParenPriority = 0; // should be less than any other priority
			constexpr priority_type additivePriority = 5;
			constexpr priority_type multiplicativePriority = 7;
			constexpr priority_type unaryPriority = 9;
			constexpr priority_type exponentiationPriority = 11;

			inline bool needsParenthesis(OpTags::OpTraits parentTraits, OpTags::OpTraits thisTraits, Child thisChild)
			{
				return parentTraits.priority > thisTraits.priority || parentTraits.priority == thisTraits.priority
					&& (parentTraits.associativity != thisTraits.associativity || thisTraits.associativity == OpTags::Associativity::LEFT
					&& thisChild == Child::RIGHT || thisTraits.associativity == OpTags::Associativity::RIGHT && thisChild == Child::LEFT);
			} // end function needsParenthesis

			// fill in function object not appearing in standard headers
			template<typename T>
			struct unary_plus
			{
				using argument_type = T;
				using result_type = T;

				T operator()(const T &x) const
				{
					return +x;
				} // end method operator()

				static OpTraits traits()
				{
					return OpTraits('+',unaryPriority,Associativity::RIGHT);
				} // end function traits
			}; // end struct unary_plus

			// extend function objects appearing in standard headers
			// with application-specific information
			template<typename T>
			struct negate : public std::negate<T>
			{
				static OpTraits traits()
				{
					return OpTraits('-',unaryPriority,Associativity::RIGHT);
				} // end function traits
			}; // end struct negate

			template<typename T>
			struct plus : public std::plus<T>
			{
				static OpTraits traits()
				{
					return OpTraits('+',additivePriority,Associativity::LEFT);
				} // end function traits
			}; // end struct plus

			template<typename T>
			struct minus : public std::minus<T>
			{
				static OpTraits traits()
				{
					return OpTraits('-',additivePriority,Associativity::LEFT);
				} // end function traits
			}; // end struct minus

			template<typename T>
			struct multiplies : public std::multiplies<T>
			{
				static OpTraits traits()
				{
					return OpTraits('*',multiplicativePriority,Associativity::LEFT);
				} // end function traits
			}; // end struct multiplies

			template<typename T>
			struct divides : public std::divides<T>
			{
				static OpTraits traits()
				{
					return OpTraits('/',multiplicativePriority,Associativity::LEFT);
				} // end function traits
			}; // end struct divides

			template<typename T>
			struct bit_xor : public std::bit_xor<T>
			{
				static OpTraits traits()
				{
					return OpTraits('^',exponentiationPriority,Associativity::RIGHT);
				} // end function traits
			}; // end struct bit_xor

		} // end namespace OpTags



		/** A class to represent free-form mathematical expressions.
		 *  Unsigned integral (natural) numbers are represented internally as UIntType.
		 *	Signed, rational and fixed point real numbers are represented using negation,
		 *	division and natural numbers.
		 */
		template<typename UIntType = unsigned long long int, typename NameType = std::string, typename IDType = size_t>
		class Expression
		{
			/**********************
			*    Concept Check    *
			**********************/

			static_assert(std::numeric_limits<UIntType>::is_integer && !std::numeric_limits<UIntType>::is_signed,
				"Expression can only be instantiated with unsigned integral types!");

			/*********************
			*    Member Types    *
			*********************/
		public:
			using uint_type = UIntType;
			using name_type = NameType;
			using id_type = IDType;
			using symbol_table_type = Common::SymbolTable<NameType,IDType>;

		private:
			struct Extends
			{
				// Fields
				size_t width;
				// height == aboveBaseLine + belowBaseLine + 1
				size_t aboveBaseLine;
				size_t belowBaseLine;

				Extends() = default;

				Extends(size_t width, size_t aboveBaseLine, size_t belowBaseLine)
					:width(width),aboveBaseLine(aboveBaseLine),belowBaseLine(belowBaseLine)
				{
					// empty body
				} // end Extends constructor
			}; // end struct Extends

			using extends_container = std::vector<Extends>; // I would like to make that a template parameter...
			static constexpr  char quotientSymbol = '_'; // should consider using a nicer (Unicode) character...
				// '_' is too low and '-' creates ambiguities with minus which must be resolved with extra parenthesis...

			struct AbstractNode
			{
			public:
				virtual void print1D(std::ostream &out, const symbol_table_type &symbols, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const = 0;
				/** Stores the extends of the rectangle, an expression subtree will occupy when
				 *	printed in 2D, in the vector extends in right postorder (right subtree before
				 *	left subtree). Returns the extends of the subtree it's called on.
				 */
				virtual Extends getPrint2DExtends(const symbol_table_type &symbols, extends_container &extends, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const = 0;
				// uses the property rev(preorder(t) == postorder(reflect(t)) to cache the node attributes outside the nodes!
				// Should probably cache the serialized literal nodes as well...
				// TODO: change top to bottom and have (0,0) be in the bottom-left corner to somewhat simplify code.
				virtual void print2D(std::vector<NameType> &out, size_t left, size_t top, const symbol_table_type &symbols, bool fullyParenthesized,
										typename extends_container::const_reverse_iterator &currentExtends, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const = 0;
				virtual std::unique_ptr<AbstractNode> deepCopy() const = 0;
				virtual ~AbstractNode() = default;
			}; // end struct AbstractNode


			struct LiteralNode : public AbstractNode
			{
				// Fields
				UIntType value;

				// Constructors / Destructor
				explicit LiteralNode(const UIntType &value)
					:value(value)
				{
					// empty body
				} // end LiteralNode constructor

				virtual ~LiteralNode() = default;

				// Methods
				virtual void print1D(std::ostream &out, const symbol_table_type &symbols, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					if(fullyParenthesized) out << '(';
					out << value;
					if(fullyParenthesized) out << ')';
				} // end method print1D

				virtual Extends getPrint2DExtends(const symbol_table_type &symbols, extends_container &extends, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					std::ostringstream sstrout;
					Extends result;

					sstrout << value;
					result = Extends(sstrout.str().size()+(fullyParenthesized<<1),0,0);

					extends.push_back(result);
					return result;
				} // end method getPrint2DExtends

				virtual void print2D(std::vector<NameType> &out, size_t left, size_t top, const symbol_table_type &symbols, bool fullyParenthesized,
										typename extends_container::const_reverse_iterator &currentExtends, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					std::ostringstream sstrout;
					sstrout << value;
					NameType strout = sstrout.str();

					std::copy(strout.begin(),strout.end(),out[top].begin() + (left+fullyParenthesized)); // can handle parenthesis
					if(fullyParenthesized)
					{
						out[top+currentExtends->aboveBaseLine][left] = '(';
						out[top+currentExtends->aboveBaseLine][left + currentExtends->width-1] = ')';
					} // end if

					++currentExtends;
				} // end method print2D

				virtual std::unique_ptr<AbstractNode> deepCopy() const
				{
					return std::unique_ptr<AbstractNode>(new LiteralNode(value));
				} // end method deepCopy
			}; // end struct LiteralNode


			struct VariableNode : public AbstractNode
			{
				// Fields
				IDType id;

				// Constructors / Destructor
				explicit VariableNode(IDType id)
					:id(id)
				{
					// empty body
				} // end VariableNode constructor

				virtual ~VariableNode() = default;

				// Methods
				virtual void print1D(std::ostream &out, const symbol_table_type &symbols, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					if(fullyParenthesized) out << '(';
					out << symbols.name(id);
					if(fullyParenthesized) out << ')';
				} // end method print1D

				virtual Extends getPrint2DExtends(const symbol_table_type &symbols, extends_container &extends, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					auto result = Extends(symbols.name(id).size() + (fullyParenthesized<<1) , 0 , 0);

					extends.push_back(result);
					return result;
				} // end method getPrint2DExtends

				virtual void print2D(std::vector<NameType> &out, size_t left, size_t top, const symbol_table_type &symbols, bool fullyParenthesized,
										typename extends_container::const_reverse_iterator &currentExtends, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					if(fullyParenthesized)
					{
						out[top][left] = '(';
						out[top][left + currentExtends->width-1] = ')';
					} // end if

					std::copy(symbols.name(id).begin(),symbols.name(id).end(),out[top].begin() + (left+fullyParenthesized));

					++currentExtends;
				} // end method print2D

				virtual std::unique_ptr<AbstractNode> deepCopy() const
				{
					return std::unique_ptr<AbstractNode>(new VariableNode(id));
				} // end method deepCopy
			}; // end struct VariableNode


			template<template<class> class Operator>
			struct UnaryNode : public AbstractNode
			{
				// Fields
				std::unique_ptr<AbstractNode> child;

				// Constructors / Destructor
				explicit UnaryNode(std::unique_ptr<AbstractNode> child)
					:child(std::move(child))
				{
					// empty body
				} // end VariableNode constructor

				virtual ~UnaryNode() = default;

				// Methods
				virtual void print1D(std::ostream &out, const symbol_table_type &symbols, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);

					if(needsParenthesis) out << '(';
					out << Operator<UIntType>::traits().symbol;
					child->print1D(out,symbols,fullyParenthesized,Operator<UIntType>::traits(),OpTags::Child::RIGHT);
					if(needsParenthesis) out << ')';
				} // end method print1D

				virtual Extends getPrint2DExtends(const symbol_table_type &symbols, extends_container &extends, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);
					Extends result = child->getPrint2DExtends(symbols,extends,fullyParenthesized,Operator<UIntType>::traits(),OpTags::Child::RIGHT);
					result.width += needsParenthesis ? 1 + 2 : 1;

					extends.push_back(result);
					return result;
				} // end method getPrint2DExtends

				virtual void print2D(std::vector<NameType> &out, size_t left, size_t top, const symbol_table_type &symbols, bool fullyParenthesized,
										typename extends_container::const_reverse_iterator &currentExtends, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);
					if(needsParenthesis)
					{
						out[top + currentExtends->aboveBaseLine][left] = '(';
						out[top + currentExtends->aboveBaseLine][left + currentExtends->width-1] = ')';
					} // end if
					out[top + currentExtends->aboveBaseLine][left+needsParenthesis] = Operator<UIntType>::traits().symbol;

					++currentExtends;

					child->print2D(out,left+1+needsParenthesis,top,symbols,fullyParenthesized,currentExtends,Operator<UIntType>::traits(),OpTags::Child::RIGHT);
				} // end method print2D

				virtual std::unique_ptr<AbstractNode> deepCopy() const
				{
					return std::unique_ptr<AbstractNode>(new UnaryNode(child->deepCopy()));
				} // end method deepCopy
			}; // end struct UnaryNode


			template<template<class> class Operator>
			struct BinaryNode : public AbstractNode
			{
				// Fields
				std::unique_ptr<AbstractNode> leftChild;
				std::unique_ptr<AbstractNode> rightChild;

				// Constructors
				BinaryNode(std::unique_ptr<AbstractNode> leftChild, std::unique_ptr<AbstractNode> rightChild)
					:leftChild(std::move(leftChild)),rightChild(std::move(rightChild))
				{
					// empty body
				} // end BinaryNode constructor

				virtual ~BinaryNode() = default;

				// Methods
				virtual void print1D(std::ostream &out, const symbol_table_type &symbols, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);

					if(needsParenthesis) out << '(';
					leftChild->print1D(out,symbols,fullyParenthesized,Operator<UIntType>::traits(),OpTags::Child::LEFT);
					out << Operator<UIntType>::traits().symbol;
					rightChild->print1D(out,symbols,fullyParenthesized,Operator<UIntType>::traits(),OpTags::Child::RIGHT);
					if(needsParenthesis) out << ')';
				} // end method print1D

				virtual Extends getPrint2DExtends(const symbol_table_type &symbols, extends_container &extends, bool fullyParenthesized, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);

					auto traits = Operator<UIntType>::traits();
					if(traits.symbol == '/' || traits.symbol == '^')
						traits.priority = OpTags::noParenPriority;	// numerator and denominator don't need parenthesis

					Extends rightExtends = rightChild->getPrint2DExtends(symbols,extends,fullyParenthesized,traits,OpTags::Child::RIGHT); // must be the right first!
					if(traits.symbol == '^')
						traits.priority = Operator<UIntType>::traits().priority; // restore priority for left child.
					Extends leftExtends = leftChild->getPrint2DExtends(symbols,extends,fullyParenthesized,traits,OpTags::Child::LEFT);
					Extends result;

					if(Operator<UIntType>::traits().symbol == '/')
					{
						// size of fraction bar disambiguates instead of parenthesis!
						result.width = std::max(rightExtends.width+((typeid(*rightChild) == typeid(BinaryNode<OpTags::divides>) && !fullyParenthesized)<<1) ,
												leftExtends.width+((typeid(*leftChild) == typeid(BinaryNode<OpTags::divides>) && !fullyParenthesized)<<1)) + (fullyParenthesized<<1);
						result.aboveBaseLine = leftExtends.aboveBaseLine + leftExtends.belowBaseLine + 1;
						result.belowBaseLine = rightExtends.aboveBaseLine + rightExtends.belowBaseLine + 1;
					}
					else if(Operator<UIntType>::traits().symbol == '^')
					{
						result.width = leftExtends.width + rightExtends.width + (needsParenthesis<<1);
						result.aboveBaseLine = leftExtends.aboveBaseLine + rightExtends.aboveBaseLine + rightExtends.belowBaseLine + 1;
						result.belowBaseLine = leftExtends.belowBaseLine;
					}
					else
					{
						result.width = rightExtends.width + leftExtends.width + (needsParenthesis<<1) + (Operator<UIntType>::traits().symbol == '*' ? 1 : 3);
						result.aboveBaseLine = std::max(rightExtends.aboveBaseLine,leftExtends.aboveBaseLine);
						result.belowBaseLine = std::max(rightExtends.belowBaseLine,leftExtends.belowBaseLine);
					} // end else

					extends.push_back(result);
					return result;
				} // end method getPrint2DExtends

				virtual void print2D(std::vector<NameType> &out, size_t left, size_t top, const symbol_table_type &symbols, bool fullyParenthesized,
										typename extends_container::const_reverse_iterator &currentExtends, OpTags::OpTraits parentTraits, OpTags::Child thisChild) const
				{
					bool needsParenthesis = fullyParenthesized || OpTags::needsParenthesis(parentTraits,Operator<UIntType>::traits(),thisChild);

					auto &thisExtends = *currentExtends;
					++currentExtends;

					if(needsParenthesis)
					{
						out[top + thisExtends.aboveBaseLine][left] = '(';
						out[top + thisExtends.aboveBaseLine][left + thisExtends.width-1] = ')';
					} // end if

					if(Operator<UIntType>::traits().symbol == '/')
					{
						auto traits = Operator<UIntType>::traits();
						traits.priority = OpTags::noParenPriority;
						std::fill_n(out[top+thisExtends.aboveBaseLine].begin()+(left+fullyParenthesized),thisExtends.width-(fullyParenthesized<<1),quotientSymbol);
						leftChild->print2D(out,left + ((thisExtends.width-currentExtends->width+1) >> 1),top,
											symbols,fullyParenthesized,currentExtends,traits,OpTags::Child::LEFT); // numerator does not need parenthesis...
						rightChild->print2D(out,left + ((thisExtends.width-currentExtends->width+1) >> 1),top+thisExtends.aboveBaseLine+1,
											symbols,fullyParenthesized,currentExtends,traits,OpTags::Child::RIGHT); // ...neither denominator (unless fully parenthesized)
					}
					else if(Operator<UIntType>::traits().symbol == '^')
					{
						leftChild->print2D(out,left+needsParenthesis,top + (thisExtends.aboveBaseLine-currentExtends->aboveBaseLine),
											symbols,fullyParenthesized,currentExtends,Operator<UIntType>::traits(),OpTags::Child::LEFT);
						rightChild->print2D(out,left+thisExtends.width-needsParenthesis - currentExtends->width,top,
											symbols,fullyParenthesized,currentExtends,Operator<UIntType>::traits().setPriority(OpTags::noParenPriority),OpTags::Child::RIGHT);
					}
					else
					{
						if(Operator<UIntType>::traits().symbol == '*')
							out[top + thisExtends.aboveBaseLine][left+needsParenthesis+currentExtends->width] = Operator<UIntType>::traits().symbol;
						else
						{
							out[top + thisExtends.aboveBaseLine][left+needsParenthesis+currentExtends->width] = ' ';
							out[top + thisExtends.aboveBaseLine][left+needsParenthesis+1+currentExtends->width] = Operator<UIntType>::traits().symbol;
							out[top + thisExtends.aboveBaseLine][left+needsParenthesis+2+currentExtends->width] = ' ';
						} // end else
						leftChild->print2D(out,left+needsParenthesis,top + (thisExtends.aboveBaseLine-currentExtends->aboveBaseLine),
											symbols,fullyParenthesized,currentExtends,Operator<UIntType>::traits(),OpTags::Child::LEFT);
						rightChild->print2D(out,left+thisExtends.width-needsParenthesis - currentExtends->width,
											top + (thisExtends.aboveBaseLine-currentExtends->aboveBaseLine),
											symbols,fullyParenthesized,currentExtends,Operator<UIntType>::traits(),OpTags::Child::RIGHT);
					} // end else
				} // end method print2D

				virtual std::unique_ptr<AbstractNode> deepCopy() const
				{
					return std::unique_ptr<AbstractNode>(new BinaryNode(leftChild->deepCopy(),rightChild->deepCopy()));
				} // end method deepCopy
			}; // end struct BinaryNode


			/***************
			*    Fields    *
			***************/

			std::shared_ptr<symbol_table_type> symbols; // should never be a null shared pointer, due to getSymbols method
			std::unique_ptr<AbstractNode> expressionTree;


			/**********************************
			*    Constructors / Destructor    *
			**********************************/
		public:
			/** Construct an empty Expression
			 */
			explicit Expression(std::shared_ptr<symbol_table_type> symbolTable = std::make_shared<symbol_table_type>())
				:symbols(std::move(symbolTable))
			{
				// empty body
			} // end Expression default constructor

			Expression(const Expression &other)
				:symbols(other.symbols),expressionTree(other.expressionTree->deepCopy())
			{
				// empty body
			} // end Expression copy constructor

			Expression(Expression &&other)
				:symbols(std::move(other.symbols)),expressionTree(std::move(other.expressionTree))
			{
				// empty body
			} // end Expression move constructor

			/** Construct an expression containing a single literal value
			 */
			Expression(const UIntType &literalValue, std::shared_ptr<symbol_table_type> symbolTable = std::shared_ptr<symbol_table_type>(new symbol_table_type()))
				:symbols(std::move(symbolTable)),expressionTree(new LiteralNode(literalValue))
			{
				// empty body
			} // end Expression constructor

			/** Construct an expression containing a single symbol.
			 *	The symbolTable argument plays the role of the namespace.
			 */
			Expression(const NameType &variableName, std::shared_ptr<symbol_table_type> symbolTable = std::shared_ptr<symbol_table_type>(new symbol_table_type()))
				:symbols(std::move(symbolTable))
			{
				expressionTree.reset(new VariableNode(symbols->declare(variableName)));
			} // end Expression constructor

			/** Construct an expression object from a sequence of characters.
			 *	The constructor parses the sequence as if it was a string and
			 *	creates an expression object representing the string.
			 *
			 *	Spirit does not support unique_ptrs so this may leak in case exceptions are thrown...
			 */
			template<typename ForwardIterator>
			Expression(ForwardIterator begin, ForwardIterator end, std::shared_ptr<symbol_table_type> symbolTable = std::shared_ptr<symbol_table_type>(new symbol_table_type()))
				:symbols(std::move(symbolTable))
			{
				using token_type = lex::lexertl::token<ForwardIterator,boost::mpl::vector<NameType>,boost::mpl::false_>;
				using lexer_type = lex::lexertl::actor_lexer<token_type>;
				using iterator_type = typename lexer_type::iterator_type;

				std::stringstream sout;
				Grammar<lexer_type> scanner(sout);
				Syntax<iterator_type> parser(scanner,sout);
				AbstractNode *parseTree = nullptr;

				bool success = lex::tokenize_and_parse(begin,end,scanner,parser(symbols.get()),parseTree);
				expressionTree.reset(parseTree);

				if(!success || begin != end)
					throw std::runtime_error(sout.str().empty() ? "Parse error reading mathematical expression!" : sout.str());
			} // end Expression constructor

			~Expression() = default;


			/****************
			*    Methods    *
			****************/

			const symbol_table_type &getSymbols() const
			{
				return *symbols;
			} // end method getSymbols

			/** Checks whether an expression is empty. i.e. Constructed with the default constructor
			 *	and not assigned a non-empty expression.
			 */
			bool empty() const
			{
				return !expressionTree;
			} // end function empty

			void print1D(std::ostream &out, bool fullyParenthesized = false) const
			{
				if(expressionTree) expressionTree->print1D(out,*symbols,fullyParenthesized,OpTags::OpTraits('+',OpTags::noParenPriority,OpTags::Associativity::RIGHT),OpTags::Child::RIGHT);
			} // end method print1D

			void print2D(std::ostream &out, bool fullyParenthesized = false) const
			{
				if(expressionTree)
				{
					extends_container extends;
					Extends rootExtends = expressionTree->getPrint2DExtends(*symbols,extends,fullyParenthesized,OpTags::OpTraits('+',OpTags::noParenPriority,OpTags::Associativity::RIGHT),OpTags::Child::RIGHT);

					std::vector<NameType> temporaryStorage(rootExtends.aboveBaseLine+rootExtends.belowBaseLine+1);
					for(NameType &row : temporaryStorage)
						row.resize(rootExtends.width,' ');

					auto iterator = extends.crbegin();
					expressionTree->print2D(temporaryStorage,0,0,*symbols,fullyParenthesized,iterator,OpTags::OpTraits('+',OpTags::noParenPriority,OpTags::Associativity::RIGHT),OpTags::Child::RIGHT);

					std::copy(temporaryStorage.begin(),temporaryStorage.end(),std::ostream_iterator<NameType>(out,"\n"));
				} // end if
			} // end method print2D

			/** Construct an expression object form a smaller one and a unary operator
			 */
			template<template<class> class Operator>
			static Expression unaryCombine(Expression subExpression)
			{
				if(subExpression.empty())
					throw std::logic_error("Empty Expression objects cannot be used to construct larger Expression objects!");
				subExpression.expressionTree.reset(new UnaryNode<Operator>(std::move(subExpression.expressionTree)));
				return subExpression;
			} // end static method unaryCombine

			/** Construct an expression object form two smaller ones and a binary operator
			 */
			template<template<class> class Operator>
			static Expression binaryCombine(Expression leftSubExpression, Expression rightSubExpression)
			{
				if(leftSubExpression.empty() || rightSubExpression.empty())
					throw std::logic_error("Empty Expression objects cannot be used to construct larger Expression objects!");
				if(leftSubExpression.symbols->empty())
					leftSubExpression.symbols = std::move(rightSubExpression.symbols);
				else if(leftSubExpression.symbols != rightSubExpression.symbols && !rightSubExpression.symbols->empty())
					throw std::logic_error("Combining sub-expressions with different symbol tables (or namespaces) is not supported!");
				leftSubExpression.expressionTree.reset(new BinaryNode<Operator>(std::move(leftSubExpression.expressionTree),std::move(rightSubExpression.expressionTree)));
				return leftSubExpression;
			} // end static method binaryCombine


			/******************
			*    Operators    *
			******************/

			Expression &operator=(Expression other)
			{
				symbols = std::move(other.symbols);
				expressionTree = std::move(other.expressionTree);
				return *this;
			} // end method operator=


			/***********************
			*    Parser Support    *
			***********************/
		private:
			template<typename Lexer>
			struct Grammar : public lex::lexer<Lexer>
			{
				// Tokens
				lex::token_def<NameType> identifier;
				lex::token_def<NameType> rationalLiteral;
				lex::token_def<NameType> whiteSpace;
				lex::token_def<NameType> illegalCharacter;

				// Regular Expressions
				/** A Grammar object should not outlive the std::ostream
				 *	one given as argument to its constructor.
				 */
				explicit Grammar(std::ostream &errorStream)
					:identifier("[a-zA-Z_][a-zA-Z_0-9]*"),
					rationalLiteral("[0-9]+(\\.[0-9]+)?"),
					whiteSpace("[ \t\v\f\n\r]"),
					illegalCharacter(".")
				{
					this->self
						= identifier | rationalLiteral
						| '(' | ')' | '+' | '-' | '*' | '/' | '^'
						| whiteSpace[lex::_pass = lex::pass_flags::pass_ignore]
						| illegalCharacter[errorStream << phoenix::val("Ignoring invalid character '") << *lex::_start << "' in input!\n"
											, lex::_pass = lex::pass_flags::pass_ignore];
							// should be pass_fail but spirit parser loops forever on failure...
				} // end Grammar constructor
			}; // end struct Grammar

			using attribute_signature = AbstractNode *(symbol_table_type *);

			template<typename ForwardIterator>
			struct Syntax : public qi::grammar<ForwardIterator,attribute_signature>
			{
				// Operator parsers
				struct AdditiveOperator : public qi::symbols<char,AbstractNode *(*)(AbstractNode *,AbstractNode *)>
				{
					AdditiveOperator()
					{
						this->add("+",binaryCombine<OpTags::plus>)
								 ("-",binaryCombine<OpTags::minus>);
					} // end AdditiveOperator constructor
				} additiveOperator; // end struct AdditiveOperator

				struct MultiplicativeOperator : public qi::symbols<char,AbstractNode *(*)(AbstractNode *,AbstractNode *)>
				{
					MultiplicativeOperator()
					{
						this->add("*",binaryCombine<OpTags::multiplies>)
								 ("/",binaryCombine<OpTags::divides>);
					} // end MultiplicativeOperator constructor
				} multiplicativeOperator; // end struct MultiplicativeOperator

				struct ExponentiationOperator : public qi::symbols<char,AbstractNode *(*)(AbstractNode *,AbstractNode *)>
				{
					ExponentiationOperator()
					{
						this->add("^",binaryCombine<OpTags::bit_xor>);
					} // end ExponentiationOperator constructor
				} exponentiationOperator; // end struct ExponentiationOperator

				struct PrefixOperator : public qi::symbols<char,AbstractNode *(*)(AbstractNode *)>
				{
					PrefixOperator()
					{
						this->add("+",unaryCombine<OpTags::unary_plus>)
								 ("-",unaryCombine<OpTags::negate>);
					} // end PrefixOperator constructor
				} prefixOperator; // end struct PrefixOperator

				// Non-Terminals
				qi::rule<ForwardIterator,attribute_signature> start;
				qi::rule<ForwardIterator,attribute_signature> expression;
				qi::rule<ForwardIterator,attribute_signature> term;
				qi::rule<ForwardIterator,attribute_signature> factor;
				qi::rule<ForwardIterator,attribute_signature> prefix;
				qi::rule<ForwardIterator,attribute_signature> primary;

				// Rules
				/** A Syntax object should not outlive the std::ostream and
				 *	Grammar ones given as arguments to its constructor.
				 */
				template<typename Lexer>
				Syntax(const Grammar<Lexer> &g, std::ostream &errorStream)
					:Syntax::base_type(start)
				{
					using qi::_val;
					using qi::_r1;
					using qi::_1;
					using qi::_2;
					using qi::_3;
					using qi::_4;
					using qi::eps;
					using phoenix::val;
					using phoenix::bind;
					using phoenix::construct;

					start = expression(_r1)[_val = _1] | eps[_val = val(nullptr)];
					expression = term(_r1)[_val = _1] > *(additiveOperator > term(_r1))[_val = bind(_1,_val,_2)];
					term = factor(_r1)[_val = _1] > *(multiplicativeOperator > factor(_r1))[_val = bind(_1,_val,_2)];
					factor = (prefixOperator > factor(_r1))[_val = bind(_1,_2)] | prefix(_r1)[_val = _1];
					prefix = (primary(_r1) >> exponentiationOperator >> prefix(_r1))[_val = bind(_2,_1,_3)] | primary(_r1)[_val = _1];	// parses more expressions
					primary = g.rationalLiteral[_val = bind(stringToRational,_1)] // than I can currently bring to canonical form
							| g.identifier[_val = bind(createVariable,_1,_r1)]	// but I should issue proper error messages from transformation routines.
							| '(' > expression(_r1)[_val = _1] > ')';

					start.name("expression-start-non-terminal");
					expression.name("expression");
					term.name("term");
					factor.name("factor");
					prefix.name("prefix-expression");
					primary.name("primary-expression");

					qi::on_error<qi::fail>
					(
						expression, errorStream << val("Parse error: \"") << construct<std::string>(_1, _2) << "\"!\n"
												<< val("Here:         ") << construct<std::string>(phoenix::bind(std::distance<ForwardIterator>,_1,_3),'-') << "^\n"
												<< "Expected a " << _4 << ".\n"
					);
				} // end Syntax constructor

			private:
				// Boost::Spirit and Boost::Phoenix workarounds:
				template<template<class> class Operator>
				static AbstractNode *unaryCombine(AbstractNode *subExpression)
				{
					return new UnaryNode<Operator>(std::unique_ptr<AbstractNode>(subExpression));
				} // end function unaryCombine

				template<template<class> class Operator>
				static AbstractNode *binaryCombine(AbstractNode *leftSubExpression, AbstractNode *rightSubExpression)
				{
					return new BinaryNode<Operator>(std::unique_ptr<AbstractNode>(leftSubExpression),std::unique_ptr<AbstractNode>(rightSubExpression));
				} // end function binaryCombine

				static AbstractNode *createVariable(const NameType &name, symbol_table_type *symbolTable)
				{
					return new VariableNode(symbolTable->declare(name));
				} // end function createVariable

				/**	s must be a string of digits [0-9] containing zero or one '.'.
				 */
				static AbstractNode *stringToRational(const NameType &s)
				{
					UIntType numerator = 0, denominator = 1;
					auto inBegin = s.begin();
					auto inEnd = s.end();

					while(inBegin != inEnd && *inBegin != '.')
					{
						numerator *= 10;
						numerator += *inBegin - '0';
						++inBegin;
					} // end while
					if(inBegin != inEnd)
						++inBegin;
					while(inBegin != inEnd)
					{
						numerator *= 10;
						numerator += *inBegin - '0';
						denominator *= 10;
						++inBegin;
					} // end while

					if(denominator != 1)
						return binaryCombine<OpTags::divides>(new LiteralNode(numerator),new LiteralNode(denominator));
					else
						return new LiteralNode(numerator);
				} // end function sequenceToRational

			}; // end struct Syntax

		}; // end class Expression

		// out-of-class initializations
		template<typename UIntType, typename NameType, typename IDType>
		constexpr char Expression<UIntType,NameType,IDType>::quotientSymbol;

		class Relation;
		class RelationSystem;

	} // end namespace FreeForms

	namespace CanonicalForms
	{


	} // end namespace CanonicalForms

	namespace DSEL
	{
		enum class Primitive {Variable,Constant};

		// Support Metafunction
		template<Primitive primitive,typename UIntType, typename NameType> struct ArgType;
		template<typename UIntType, typename NameType>
		struct ArgType<Primitive::Variable,UIntType,NameType>{using type = NameType;};
		template<typename UIntType, typename NameType>
		struct ArgType<Primitive::Constant,UIntType,NameType>{using type = UIntType;};

		template<Primitive primitive, typename UIntType = unsigned long long int, typename NameType = std::string, typename IDType = size_t>
		struct ExpressionBuilder
		{
			// Member Types
			using uint_type = UIntType;
			using name_type = NameType;
			using id_type = IDType;
			using expression_type = FreeForms::Expression<UIntType,NameType,IDType>;
			using symbol_table_type = typename expression_type::symbol_table_type;

			// Fields
		public:
			static const Primitive build_primitive = primitive;
		private:
			std::shared_ptr<symbol_table_type> spSymbolTable;

			// Constructors / Destructor
		public:
			explicit ExpressionBuilder(std::shared_ptr<symbol_table_type> spSymbolTable = std::make_shared<symbol_table_type>())
				:spSymbolTable(std::move(spSymbolTable))
			{
				// empty body
			} // end ExpressionBuilder default constructor

			// Methods
			expression_type operator()(const typename ArgType<primitive,UIntType,NameType>::type &argument)
			{
				return expression_type(argument,spSymbolTable);
			} // end method operator()
		}; // end struct template ExpressionBuilder


#define UNARY_EXPRESSION(op,tag) \
		template<typename UIntType, typename NameType, typename IDType> \
		inline FreeForms::Expression<UIntType,NameType,IDType> operator op (FreeForms::Expression<UIntType,NameType,IDType> subExpression) \
		{\
			return FreeForms::Expression<UIntType,NameType,IDType>::template unaryCombine<tag>(std::move(subExpression));\
		} // end function operator op

		UNARY_EXPRESSION(+,FreeForms::OpTags::unary_plus);
		UNARY_EXPRESSION(-,FreeForms::OpTags::negate);
#undef UNARY_EXPRESSION

#define BINARY_EXPRESSION(op,tag) \
		template<typename UIntType, typename NameType, typename IDType> \
		inline decltype(auto) operator op(FreeForms::Expression<UIntType,NameType,IDType> leftSubExpression, FreeForms::Expression<UIntType,NameType,IDType> rightSubExpression) \
		{\
			return FreeForms::Expression<UIntType,NameType,IDType>::template \
				binaryCombine<tag>(std::move(leftSubExpression),std::move(rightSubExpression));\
		} /* end function operator op*/\
		\
		template<typename UIntType, typename NameType, typename IDType, typename OtherOpType> \
		inline decltype(auto) operator op(FreeForms::Expression<UIntType,NameType,IDType> leftSubExpression, OtherOpType &&rightSubExpression) \
		{\
			return FreeForms::Expression<UIntType,NameType,IDType>::template \
				binaryCombine<tag>(std::move(leftSubExpression),decltype(leftSubExpression)(std::forward<OtherOpType>(rightSubExpression)));\
		} /* end function operator op*/\
		\
		template<typename UIntType, typename NameType, typename IDType, typename OtherOpType> \
		inline decltype(auto) operator op(OtherOpType &&leftSubExpression, FreeForms::Expression<UIntType,NameType,IDType> rightSubExpression) \
		{\
			return FreeForms::Expression<UIntType,NameType,IDType>::template \
				binaryCombine<tag>(decltype(rightSubExpression)(std::forward<OtherOpType>(leftSubExpression)),std::move(rightSubExpression));\
		} /* end function operator op*/

		BINARY_EXPRESSION(+,FreeForms::OpTags::plus);
		BINARY_EXPRESSION(-,FreeForms::OpTags::minus);
		BINARY_EXPRESSION(*,FreeForms::OpTags::multiplies);
		BINARY_EXPRESSION(/,FreeForms::OpTags::divides);
		BINARY_EXPRESSION(^,FreeForms::OpTags::bit_xor);
#undef BINARY_EXPRESSION
	} // end namespace DSEL

} // end namespace Symbolic

#endif // SYMBOLIC_COMPUTATION_H
