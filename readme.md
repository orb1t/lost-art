# LostArt GUI Builder

A WYSIWYG graphical user interface builder for creating a form of visual specification that resembles
an engineering drawing, as well as generating source code that implements it.

The repository also hosts a prototype UX library called *Graphene* which enables
building widgets by stacking smaller components called *frames*.

[![GitHub Tag](https://img.shields.io/github/tag/avnomad/lost-art.svg?maxAge=86400)](#)
[![Build Status](https://travis-ci.org/avnomad/lost-art.svg?branch=default)](https://travis-ci.org/avnomad/lost-art)
[![Coverage Status](https://coveralls.io/repos/github/avnomad/lost-art/badge.svg?branch=default)](https://coveralls.io/github/avnomad/lost-art?branch=default)

## Introduction

There are many ways to create a user interface:

* Traditional WYSIWYG, drag-and-drop [GUI builders][1] allow high productivity
  and can be used by individuals with little or no programming skills. The
  downside is that there is no way to express how the interface should adapt
  when the window size or screen resolution change, or how to deal with high
  pixel densities like those common in mobile devices.
* Using a [windowing toolkit][2] directly from within a programming environment
  or complementing the code generated by a WYSIWYG GUI builder with 'resize'
  event handlers allows unlimited flexibility, but is quite involved and
  requires decent programming skills.
* [User interface markup languages][3] generally allow one to express more UI
  properties in a declarative way than WYSIWYG GUI builders, but they are also
  more involved and still not expressive enough to obviate the need to program
  resize handlers.
* [Layout managers][4] behave well when the window is resized, but they are not
  easily usable through WYSIWYG GUI builders and certain layouts may be
  impossible to express with existing ones.

[1]: https://en.wikipedia.org/wiki/Graphical_user_interface_builder
[2]: https://en.wikipedia.org/wiki/Widget_toolkit
[3]: https://en.wikipedia.org/wiki/User_interface_markup_language
[4]: https://en.wikipedia.org/wiki/Layout_manager

LostArt was envisioned as a tool that would be more expressive than a
traditional GUI builder, yet require less time and expertise to use than a
programming or user interface markup language.

Such a tool would need to:

 1. Enable quick specification of the user interface contents by dragging-and-
	dropping widgets from a toolbox.
 2. Facilitate development of applications that adapt to different screen and
	window sizes and pixel densities by using high-level constraints like:
	  - These text boxes should have the same height.
	  - This label should be aligned to the right ⅓ of the window.
	  - This button should be large enough to be pressed by finger.
	  - This text area should be at most 80 ems wide.
 3. Generate highly optimized code that is not only as fast, but also as
	readable as the one a human would write for the same user interface.
 4. Include a variety of back-ends for different programming languages (C++,
	Javascript, ...), windowing toolkits (GTK, Qt, HTML/CSS, ...) and platforms
	(desktop, mobile, web, ...).
 5. Understand the specification well enough to produce meaningful errors and
	warnings like those a compiler would. e.g.
	  - If the specified UI works only on screens with 96 PPI.
	  - If the specification implies a window that is not resizable.
	  - If a widget jumps back and forth while the window is resized.
 6. Support mixing linear and multi-way constraints.
 7. Allow the definition of interfaces in a modular way that enables reuse.
 8. Be cross platform, with support for multiple host and target operating
	systems.
 9. Save files in an open, plain text format that could be put under source
	control.

But, perhaps more importantly, it would need to define a graphical language that
would enable software engineers to specify the contents and behavior of their
user interface in a single sheet of paper that could later be printed and used
to communicate those ideas to other engineers. Just like mechanical engineers
can use [engineering drawings][5] to communicate the shape and size of physical
objects with each other.

[5]: https://en.wikipedia.org/wiki/Engineering_drawing

## Related work

There has been a lot of research around the use of constraints — both linear
\[[3][r3], [4][r4], [7][r7]\] and multi-way \[[1][r1], [2][r2]\] — in user
interface development in the 90s and later. Some require a solver to be present
in the execution environment \[[3][r3], [5][r5], [6][r6]\], while others compile
constraints in a stand-alone application \[[4][r4]\]. Some goes beyond user
interface construction and into web \[[5][r5]\] and window \[[6][r6]\] layout.

1. [Multi-way versus One-way Constraints in User Interfaces Experience with the
	DeltaBlue Algorithm (1993)][r1]
2. [Skyblue: a multi-way local propagation constraint solver for user interface
	construction (1994)][r2]
3. [Solving Linear Arithmetic Constraints for User Interface Applications
	(1997)][r3]
4. [Compiling Constraint Solving using Projection (1997)][r4]
5. [Constraint-Based Document Layout for the Web (2000)][r5]
6. [Scwm - An Extensible Constraint-Enabled Window Manager (2001)][r6]
7. [Domain Specific High-Level Constraints for User Interface Layout (2008)][r7]

LostArt adopts the compilation approach, but attempts to create a visual
language where constraints can be added and edited directly on the UI canvas
instead of being listed as equations in a separate pane. Moreover min/max
constraints and inequalities, when supported, will be based on parametric linear
programming instead of a numeric solver. Finally there are no plans to support
constraint hierarchies (at least not in the way the aforementioned projects did)
and in the context of multi-way constraints that may mean that the direction in
which information will flow is known beforehand and better complexity guarantees
can be achieved.

[r1]: http://doi.wiley.com/10.1002/spe.4380230507
[r2]: http://dl.acm.org/citation.cfm?id=192426.192485
[r3]: http://portal.acm.org/citation.cfm?doid=263407.263518
[r4]: http://link.springer.com/10.1007/BFb0017462
[r5]: http://link.springer.com/10.1007/s005300000043
[r6]: http://www.usenix.org/events/usenix01/freenix01/badros.html
[r7]: http://link.springer.com/10.1007/s10601-008-9043-2

## Prerequisites

This is a C++14 project, so you'll need a standards compliant compiler in order
to build it. You'll also need:

 1. [CMake](https://cmake.org/) (version 3.5 or later)
 2. [Boost](https://www.boost.org/) (version 1.56 or later)
 3. OpenGL (usually installed together with your graphics driver)
 4. [GLEW](http://glew.sourceforge.net/)
 5. [FreeGLUT](http://freeglut.sourceforge.net/) (version 2.8.1 or later)

## Build

From the root of your local clone of the repository (the directory with the .hg
or .git subdirectory) execute the following platform-independent commands:

	cmake -E make_directory build
	cmake -E chdir build cmake ..
	cmake -E chdir build cmake --build .

To also build and run the unit tests, add the following command at the end:

	cmake -E chdir build cmake --build . --target unit-tests

Depending on your choice of operating system and compiler and the way you
installed the prerequisites, you might need to add some additional options to
the `cmake ..` command and/or define some environment variables before it.

e.g. Using [vcpkg][6] on Windows will require adding the following option:

	-DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg-installation]\scripts\buildsystems\vcpkg.cmake

while selecting [clang][7] on Linux will require changing the CC and CXX
variables beforehand like so:

	export CC=clang
	export CXX=clang++

[6]: https://docs.microsoft.com/en-us/cpp/vcpkg
[7]: https://clang.llvm.org/

## Usage

### Launching LostArt

The easiest way to run the project is by executing the following command after
building it:

	cmake -E chdir build cmake --build . --target run

Alternatively, you can find the compiled executable under `build/binaries` in a
subdirectory that depends on the tools you used to build the project.

LostArt is a GUI application, but currently error messages are only displayed in
the terminal. On Windows a dedicated terminal window will open automatically,
but on Linux you have to make sure you execute LostArt from an existing
terminal, otherwise you won't be able to see error and warning messages.

### Creating a UI

The interaction is relatively simple. Below all clicks are left-clicks:

  - Clicking on empty space and dragging creates rectangles representing
	controls/widgets.
  - Clicking inside an existing control and dragging moves it.
  - Clicking on an individual side or corner and dragging moves the
	corresponding side or corner.
  - Clicking anywhere inside an existing control also gives it input focus and
	selects it.
	  - You can type ASCII characters, delete them using the `<backspace>` or
		`<delete>` keys and move your caret using the `<left arrow>` and
		`<right arrow>` keys.
	  - The `<escape>` key removes input focus.
  - Pressing the `<escape>` key when nothing has the input focus closes the
	application.
  - Pressing the `<delete>` key when nothing has the input focus deletes the
	current selection.
  - Clicking on a control side while the `<ctrl>` key is pressed, selects it.
  - Selecting a control side parallel to one already selected, creates a
	constraint on those two sides.
	  - You can move a constraint, delete it or edit its formula just like you
		would with a control.
	  - A constraint formula must be of the form `c1 v1 + c2 v2 + ... + cn vn`.
		Where
		  - `c1,...,cn` are decimal or fractional numbers, possibly negative.
		  - `v1,...,vn` are variable names or `px` or `mm`. The latter are
			interpreted as units. (pixels and millimetres respectively)
		  - e.g. `1/2a+-b+2px+-2.2mm`
	  - Variables with the same name in different constraint formulas are
		considered to be the same variable.

### Loading and Saving a UI

LostArt's interface has a single text box at the top that contains the relative
or absolute path to an XML file describing a UI.

Clicking `Load` will replace the interface currently being edited with the file
contents. An error will be printed if the file doesn't exist.

Clicking `Save` will replace the file contents with the interface currently
being edited, or create a new file if one doesn't exist.

### Compiling and Running a UI

Clicking `Compile` will check the current interface specification and print
error and/or warning messages as needed. If there are no errors, it will proceed
to create an `application customization.hpp` header in the working directory and
build it. The latter header will contain an implementation of the specified
interface.

Clicking `Run` will invoke the generated application, so one can interact with
it and verify it behaves as expected when resized.

Note that currently LostArt expects to find a few files in its working directory
(namely `CMakeLists.txt`, `glut-application.cpp` and `geometry.hpp`) when
compiling and running specifications.

If you run it through `cmake`, the latter will take care of setting the working
directory correctly.

Also note that LostArt depends on CMake to build the source code it generates so
the latter must be in the system path.

If you are not satisfied with CMake's default choices of compiler/generator or
CMake is unable to locate where you've installed the required libraries, you
must create a 'build' directory manually in LostArt's working directory and
preconfigure the CMake project accordingly before attempting to click the
`Compile` button in LostArt's interface.

## Examples

LostArt comes with a number of example user interface specifications that can
help you get started and understand how to use it and for what. They are located
in the `examples` directory and each one contains a short description in the
form of an XML comment.

There is also a video available at <https://vimeo.com/avnomad/lostart-dialog-demo>

## Compatibility

You should be able to build LostArt using any standards-compliant C++14 compiler
and run it on any major operating system, but the following combinations have
been explicitly tested:

- GCC 5.5.0 on Ubuntu 14.04 LTS (Trusty Tahr) 64-bit
- Clang 3.6.2 on Ubuntu 14.04 LTS (Trusty Tahr) 64-bit
- GCC 5.4.0 on Linux Mint 18 (Sarah) 64-bit
- Clang 3.8.0 on Linux Mint 18 (Sarah) 64-bit
- Visual Studio 2017 Community Edition on Windows 7 Professional 64-bit Service
  Pack 1
- Visual Studio 2017 Community Edition on Windows 8.1 Pro 64-bit

## How it works

In an abstract level, the user interface specification is converted to a
parametric right-hand-side system of linear equations that is solved
symbolically. If the system has a unique solution for all parameter values,
this solution is converted to code that computes it and linked with an
application template. Otherwise, the system properties are converted back into
an error message that is presented to the user.

The conversion to a system of equations proceeds roughly as follows:

- For each control, 4 variables are generated. One for each side.
- For each unique variable named in a constraint formula 1 variable is
  generated.
- For each constraint, 1 equation is generated. It involves the variables that
  correspond to the control sides it is attached to and any variable it names in
  its formula.
- 4 parameters are generated: 2 for the window's width and height and 2 for the
  screen's pixel density in the x and y direction. (More can be added in the
  future. e.g. for the cursor position.)
- Constraints attached to the window's right and top sides involve the window's
  width and height respectively. Left and bottom sides are considered to cross
  at the origin.
- Horizontal and vertical constraints mentioning `px` involve the horizontal and
  vertical pixel density respectively.

## Status and future work

This is currently a prototype and a lot of work is needed before it can deliver
its promise of a universal, intuitive, productive and expressive GUI builder.

The solver currently only supports linear systems of equations and needs to
gradually add parametric linear programming, multi-way constraints and modular
specifications.

The option to add certain trivial constraints needs to be added. e.g.

- When a control is first placed on the design surface, it could be constraint
  to its initial position and size using the closest window corner as an anchor.
- When a control is moved and/or resized visual guides could be presented to
  offer the generation of constraints that would force alignment relative to
  other control sides and centers.
- A toolbar could contain buttons for commonly used alignments (left, center,
  right) that would automatically generate the constraints that implement them.

Line styles and types should follow rules and have semantics similar to those of
engineering drawings. This includes rules on when lines representing constraints
are allowed to cross each other or lines representing controls. Constraints
could be placed automatically in a way that respects those rules.

Error messages are currently printed in a terminal and need to move to dialog
boxes. A lot can be done to improve the messages themselves and highlighting
the problematic parts of the specification will greatly improve the user
experience.

Currently a custom UI toolkit (Graphene) is used for LostArt which has limited
functionality and in turn limits the interactivity of LostArt itself. Features
like copy-paste that are taken for granted are missing. A more mature toolkit
could be used in the future or more resources could be devoted to the
development of Graphene.

The generated code currently depends on Graphene and linked to a GLUT
application template. In the future multiple back-ends must be supported.

There is currently no way to specify the type of widget (button, textbox, ...),
nor its properties (colour, text, tabStop, ...).

Ways to specify constraints on the ith control of a dynamic set (like one that
depends on user input) are also an option for more complex UIs.

## About the name

"Lost art" is "something usually requiring some skill that not many people do
any more". It may not be obvious how such a name would be appropriate for an
application that is more-or-less novel.

But the mathematics behind it, namely investigating an arbitrary parametric
linear system of equations to determine under which conditions it is solvable,
*are* something of a lost art. Although the techniques have been known for
centuries, very few textbooks and people today go beyond numeric solutions of
systems that have a square matrix.

## Graphene

The UI components used to build LostArt's own user interface are actually part
of an independent, and equally ambitious project called *Graphene* that is
currently hosted under the same repository.

### Introduction

Graphene is primarily a UX library, but the goal is to be versatile enough to be
used in the creation of highly customized UIs like those found on computer games
and certain creative applications. Widgets for the visualization and
manipulation of data sets (e.g. charts, graph drawing, etc.) are also within its
scope.

The main differences compared to other UX libraries are that:

 1. While in most UX libraries and frameworks events start at the leaves of the
	widget tree and can optionally be forwarded to their parents, Graphene also
	supports the inverse model of sending events to the root widget which
	forwards them to children as needed.
 2. Graphene doesn't come with a fixed set of ready-made widgets, but rather
	with a number of smaller components called *frames* that can be easily
	combined to create the desired widgets.

### Key features

- Unlimited customization. e.g. you can have 'donut' buttons that not only
  *look* like circular rings, but also *behave* like ones ignoring mouse clicks
  in their empty center.
- Easily adapt widgets to different modalities (mouse & keyboard, multitouch,
  hand tracking, etc.).
- Easily adapt to different windowing systems and toolkits through *event
  adaptors*.
- Event-driven architecture is supported, but not mandatory.
- Library not framework. Roughly speaking a framework receives control early on
  and keeps it until your application ends, providing a handful of customization
  points where you can insert your code. A library on the other hand doesn't
  dictate the architecture of your application and instead provides components
  that do some work and sooner-or-later return control to your application.

### Frames architecture

*Frames* are small classes that can be combined into *frame stacks* that
represent either concrete widgets, or interfaces to which widgets must conform.

Each *frame* provides part of the implementation of a different aspect of a
widget's look-and-feel. There are currently the following *frame* categories:

 1. *interface*: They only contain pure virtual methods and can be used either
	in interface-only stacks to build elaborate interfaces, or at the top of
	diverse stacks to ensure conformity to some interface.
 2. *stateful*: These provide different fields or groups of fields together with
	the constructors and accessors needed to initialize and manipulate them.
	Other categories do not have fields of their own and many require that some
	stateful frame is added earlier in the stack.
 3. *behavioural*: You can think of these as Finite State Machines that define
	which *messages* can change the *frame stack*'s state and how, possibly
	producing some output in the process.
 4. *renderable*: Mapping the current state to an image through a `render`
	method, these frames connect a *frame stack* with the computer's output
	subsystem.
 5. *event-handling*: These map concrete events of a given modality to abstract
	*messages* defined by a given set of behavioural frames. They connect a
	*frame stack* with the computer's input subsystem.

There is also a set of *event adaptors* that map the events provided by a
certain windowing system or toolkit to the events expected by the
*event-handling* frames of a given *modality*.

### Usage

Typically to construct a concrete *frame stack* that doesn't need to conform to
some interface, you start by adding a class representing an abstract object
like a rectangle or a polynomial. This class would typically be provided by some
other library.

Then you add a few stateful frames (e.g. for a `pressed` flag, or a `name`
string) and some behaviour (e.g. "Should a frame stack be movable?" or "Should
the text be editable?").

Finally, you select one or more modalities using the corresponding
event-handling frames (e.g. for keyboard & mouse and multitouch) and add some
renderable frames to specify how the complete *frame stack* should be drawn.

### Examples

Here is an example of how a *frame stack* for a simple 'Button' widget could
look like in C++14:

	using Button = FrameStack<
		Rectangle<int>,
		Frame<UniformlyBordered, int>,
		Frame<Pressable>,
		Frame<Highlightable>,
		Frame<Textual, std::string>,
		Frame<SizedText, GlutStrokeFontEngine>,
		Frame<BoxedAdaptableSizeText, std::ratio<11>>,
		Frame<TwoStagePressable>,
		Condition<Pressed,
			Sequence<
				Frame<Colorblind::FilledRectangle>,
				FrameStack<
					Frame<Colorblind::BoxedText, FunctionObjects::Textual>,
					Frame<Colorblind::InversedColor>>>,
			Condition<Highlighted,
				Sequence<
					Frame<Colorblind::BorderedRectangle, std::ratio<10>>,
					Frame<Colorblind::BoxedText, FunctionObjects::Textual>>,
				Sequence<
					Frame<Colorblind::BorderedRectangle, std::ratio<0>>,
					Frame<Colorblind::BoxedText, FunctionObjects::Textual>>
			>
		>
	>;

### Status and future work

Like LostArt itself, this is work in progress and a lot of obvious stuff are
missing (e.g. colours).

The [DSEL][] used to combine *frames* into *frame stacks* or *widgets* is usable,
but a lot of boilerplate code can still be removed and the syntax can be made
lighter.

Currently only GLUT fonts are implemented, which means the design for
plugging-in different font engines, has never been tested!

Event adapters for different windowing systems need to be implemented so that
the design for plugging them in can be validated.

Eventually graphical tools for combining or even building new frames will be
created.

[DSEL]: https://en.wikipedia.org/wiki/Domain-specific_language#Usage_patterns

### Why not a separate repository?

Source code is kept fairly separate, with no cyclic dependencies and the
intention is for Graphene to eventually be released as a separate project, with
its own repository and version numbers and be used by projects other than
LostArt.

However it made sense to initially develop those project together for the
following reasons:

1. LostArt needed to use a UX library anyway.
2. Graphene needed a comprehensive use case to help decide which features to
   include.
3. The API between the two changed so frequently, that it was impractical to
   produce and consume a new artifact each time a change was made.
4. There was no need for a single LostArt version to support multiple Graphene
   versions yet.

## Mirrors

Currently the project exists in two different mirrors that are kept in sync:

1. <https://github.com/avnomad/lost-art>
2. <https://bitbucket.org/avnomad/lost-art>

The 2nd presents a slightly more accurate picture of the history. That's because
I've developed the project using Mercurial and I've used a few features that
aren't available in Git. Although all the information is preserved by the hg-git
bridge, some commit meta-data will only be visible as part of the commit message
in GitHub. Moreover branch heads for which there is no bookmark are missing from
GitHub.

## Contributing

In order to contribute, just fork the project and open a pull request in either
GitHub or BitBucket.

## Notes

  - I've used Hg branches in a rather unusual way, in this project. [This][8]
	blog post attempts to explain how and why.

	Moreover the situation is slightly complicated by the fact that this
	repository currently hosts both LostArt (an application) and Graphene (a
	library) resulting to some ambiguity as to which category a commit belongs
	to. e.g. "Should this commit be considered a refactoring of the	application
	or an enhancement of the library?"

  - This project uses [Semantic Versioning][9]. Since there is no stable public
	API yet, the standard only prescribes that major version should be 0, not
	when minor and patch versions should increase. So I increase the minor
	version when significant new functionality is added (regardless of its
	effect on the API) and patch version when bugs are fixed.

[8]: http://blog.cornercase.gr/post/2018/03/22/Mercurial-branches-as-categories
[9]: https://semver.org/

## License

This is _free_ software. You can redistribute it and/or modify it under the
terms of the [GNU General Public License][10] version 3 or later.

[10]: https://www.gnu.org/licenses/gpl.html
