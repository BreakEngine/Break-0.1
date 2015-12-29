#pragma once

#include "Globals.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API RingCursor{
        public:

            s64 Lap;
            s64 Position;
            s64 Size;

            RingCursor(s64 size=0, s64 lap=0,s64 position=0){
                Lap = lap;
                Position = position;
                Size = size;
            }

            RingCursor& operator+(const s32 advance){
                if(Position + advance >= Size)
                {
                    Lap++;
                    Position = (Position+advance)%Size;
                }else{
                    Position += advance;
                }
				return *this;
            }

			s64& operator-(const RingCursor& advance){
				s64 result = 0;
				s64 LapDiff = Lap - advance.Lap;

				result += LapDiff*Size;

				result += Position - advance.Position;

				return result;
            }

            RingCursor& operator+=(const s32 advance){
                return operator+(advance);
            }

            RingCursor& operator+(const s64 advance){
                if(Position + advance >= Size)
                {
                    Lap++;
                    Position = (Position+advance)%Size;
                }else{
                    Position += advance;
                }
				return *this;
            }

            RingCursor& operator+=(const s64 advance){
                return operator+(advance);
            }

			RingCursor& operator+(const u32 advance){
                if(Position + advance >= Size)
                {
                    Lap++;
                    Position = (Position+advance)%Size;
                }else{
                    Position += advance;
                }
				return *this;
            }

            RingCursor& operator+=(const u32 advance){
                return operator+(advance);
            }

            bool operator==(const RingCursor& cursor){
                if(Lap == cursor.Lap && Position == cursor.Position)
                    return true;
                else
                    return false;
            }

            bool operator!=(const RingCursor& cursor){
                return !((*this)==cursor);
            }

            bool operator<(const RingCursor& cursor){
                if(Lap < cursor.Lap)
                    return true;
                else if(Lap > cursor.Lap)
                    return false;

                if(Position < cursor.Position)
                    return true;
                else if(Position > cursor.Position)
                    return false;

                return false;
            }

            bool operator>(const RingCursor& cursor){
                return !operator<(cursor) && !operator==(cursor);
            }

            bool operator<=(const RingCursor& cursor){
                return operator<(cursor) || operator==(cursor);
            }

            bool operator>=(const RingCursor& cursor){
                return operator>(cursor) || operator==(cursor);
            }

        };
    }
}
