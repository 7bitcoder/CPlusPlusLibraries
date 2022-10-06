
#pragma once

#include <string>
#include <vector>

namespace sd::filesystem
{
    class Path
    {
      public:
        enum Format
        {
            Native,
            Standard,
            Auto
        };

      private:
        Format _format = Auto;
        std::vector<std::string> _segments;

      public:
        static const char NativeSeparator;
        static const char StandardSeparator;

        Path() = default;
        Path(const Path &path);
        Path(Path &&path);
        Path(const std::string &path, Format fmt = Auto);
        template <class InputIt> Path(InputIt first, InputIt last, Format fmt = Auto);

        Path &operator=(const Path &path);
        Path &operator=(Path &&path);

        Path &Assign(const Path &path);
        Path &Assign(Path &&path);

        Path &operator/=(const Path &path);
        Path &operator/=(Path &&path);

        Path &Append(const Path &path);
        Path &Append(Path &&path);

        Path &operator+=(const Path &path);
        Path &operator+=(Path &&path);

        Path &Concat(const Path &path);
        Path &Concat(Path &&path);

        Path &RemoveFileName();
        Path &ReplaceFileName(const Path &path);
        Path &ReplaceExtension(const Path &path);

        std::string ToString() const;
        std::string ToNativeString() const;
        std::string ToStandardString() const;

        Path RootName() const;
        Path RootDirectory() const;
        Path RootPath() const;
        Path ParentPath() const;
        Path FileName() const;
        Path Extension() const;
        Path Stem() const;

        bool HasRootName() const;
        bool HasRootDirectory() const;
        bool HasRootPath() const;
        bool HasParentPath() const;
        bool HasFileName() const;
        bool HasExtension() const;
        bool HasStem() const;

        void Swap(Path &path);

        void Clear();

        bool Empty() const;

      private:
        static Path FromString(std::string path, Path::Format fmt = Path::Format::Auto);
        static Path FromStringSegments(std::vector<std::string> segments, Path::Format fmt = Path::Format::Auto);
    };
} // namespace sd::filesystem