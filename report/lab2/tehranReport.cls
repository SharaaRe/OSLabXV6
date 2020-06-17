\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{tehranReport}

\LoadClass{report}
\RequirePackage{kvoptions}
\RequirePackage{fancyhdr}
\RequirePackage{graphicx}
\RequirePackage{tocbibind}
\RequirePackage[dvipsnames,table]{xcolor}
\RequirePackage[colorlinks, citecolor=Green, linkcolor=Red, filecolor=Cyan, urlcolor=Magenta]{hyperref}
\usepackage{listings}
\usepackage{fontspec}

\DeclareStringOption[logo.png]{logo}
\DeclareDefaultOption{\PassOptionsToClass{\CurrentOption}{report}}
\ProcessKeyvalOptions*

\setlength\extrarowheight{15pt}

\makeatletter

\AtBeginDocument{
    \let\thetitle\@title
    \let\theauthor\@author

    % setup fancyhdr
    \pagestyle{fancy}
    \fancyhf{}
    \chead{\thecourse\quad{\bfseries \thetitle}\dotfill\thepage}

    \hypersetup{pdftitle=\thetitle}
    \hypersetup{pdfauthor=\theauthor}
    \hypersetup{pdfsubject={\thecourse{} :: \thetitle{}}}
    
    \definecolor{background}{RGB}{250, 250, 250}
    \definecolor{string}{RGB}{40, 150, 51}
    \definecolor{comment}{RGB}{169, 169, 169}
    \definecolor{normal}{RGB}{54, 54, 54}
    \definecolor{identifier}{RGB}{169, 46, 53}
    \definecolor{keyword}{RGB}{91, 77, 194}
    
    \lstset{
    	language=C,                			% choose the language of the code
    	numbers=left,                   		% where to put the line-numbers
    	stepnumber=1,                   		% the step between two line-numbers.        
    	numbersep=5pt,                  		% how far the line-numbers are from the code
    	numberstyle=\tiny\color{black}\ttfamily,
    	backgroundcolor=\color{background},  		% choose the background color. You must add \usepackage{color}
    	showspaces=false,               		% show spaces adding particular underscores
    	showstringspaces=false,         		% underline spaces within strings
    	showtabs=false,                 		% show tabs within strings adding particular underscores
    	tabsize=4,                      		% sets default tabsize to 2 spaces
    	captionpos=t,                   		% sets the caption-position to bottom
    	breaklines=true,                		% sets automatic line breaking
    	breakatwhitespace=true,         		% sets if automatic breaks should only happen at whitespace
    	%title=\lstname,                 		% show the filename of files included with \lstinputlisting;
    	basicstyle=\color{normal}\ttfamily,					% sets font style for the code
    	keywordstyle=\color{keyword}\ttfamily,	% sets color for keywords
    	% keywordstyle=[2]\color{identifier}
    	% morekeywords=[2]{load_data, estimate_params, calc_prior_probabilities, shrinkage, diff, argmax, main},
    	stringstyle=\color{string}\ttfamily,	% sets color for strings
    	commentstyle=\color{normal}\ttfamily,	% sets color for comments
    	morecomment=[f][\color{comment}][0]{//},
    	morecomment=[f][\color{normal}][0]{\#},
    	emph={format_string, eff_ana_bf, permute, eff_ana_btr},
    	emphstyle=\color{identifier}\ttfamily,
    	moredelim=**[is][\textcolor{green}]{def}{(},
    	inputpath=codes/	
    }
}

\newcommand{\maketitlepage}{
    \begin{titlepage}%
        \let\footnotesize\small
        \let\footnoterule\relax
        \let \footnote \thanks
        \null\vfil
        \vskip 20\p@
        \begin{center}%
            {\rule{0.6\textwidth}{0.5pt}\par}
            {\includegraphics[height = 7em]{\tehranReport@logo} \par}%
            {\Large \bfseries \theuniversity \par}%
            {\large \thecollege \par}%
            {\rule{0.6\textwidth}{0.5pt}\par}
            \vskip 6em%
            {\Huge \bfseries \@title \par}%
            {\rule{0.8\textwidth}{1pt}\par}
            {\Large \thecourse \par}%
            \vskip 3em%
        	{\LARGE \bfseries اعضای گروه \par}%    		
            \vskip 0.5em%
            \begin{tabular}{r c l}
            	\LARGE \@author & \large \theauthorposition & \normalsize \thestudentnumber \par \\ 
            	
            	\if\@secondmember
            	\else
            	\LARGE \@secondmember & \large \thesecondmemberposition & \normalsize \thesecondmemberstudentnumber \par \\
            	\fi
            	
            	\if\@thirdmember
            	\else
            	\LARGE \@thirdmember & \large \thethirdmemberposition & \normalsize \thethirdmemberstudentnumber \par \\
            	\fi 
            \end{tabular}
        	
            \vskip 3em%
            {\large \bfseries استاد \par}%
            \vskip 0.5em%
            {\large \thesupervisor \par}%
            \vskip 3em%
            {\@date \par}%
        \end{center}\par
        \@thanks
        \vfil\null
        \newpage
    \end{titlepage}%
    \setcounter{footnote}{0}%
    \global\let\thanks\relax
    \global\let\maketitle\relax
    \global\let\@thanks\@empty
    \global\let\@author\@empty
    \global\let\@date\@empty
    \global\let\@title\@empty
    \global\let\title\relax
    \global\let\author\relax
    \global\let\date\relax
    \global\let\and\relax
}
\renewcommand{\maketitle}{\maketitlepage}
\newcommand{\university}[1]{\def\theuniversity{#1}}
\newcommand{\college}[1]{\def\thecollege{#1}}
\newcommand{\studentNumber}[1]{\def\thestudentnumber{#1}}
\newcommand{\authorPosition}[1]{\def\theauthorposition{#1}}

\newcommand{\secondMember}[1]{\def\@secondmember{#1}}
\newcommand{\secondMemberStudentNumber}[1]{\def\thesecondmemberstudentnumber{#1}}
\newcommand{\secondMemberPosition}[1]{\def\thesecondmemberposition{#1}}
\secondMember{}

\newcommand{\thirdMember}[1]{\def\@thirdmember{#1}}
\newcommand{\thirdMemberStudentNumber}[1]{\def\thethirdmemberstudentnumber{#1}}
\newcommand{\thirdMemberPosition}[1]{\def\thethirdmemberposition{#1}}
\secondMember{}

\newcommand{\course}[1]{\def\thecourse{#1}}
\newcommand{\supervisor}[1]{\def\thesupervisor{#1}}

\makeatother
